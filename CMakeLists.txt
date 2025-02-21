cmake_minimum_required(VERSION 3.21)
message("Using toolchain file ${CMAKE_TOOLCHAIN_FILE}.")

########################################################################################################################
## Define project
########################################################################################################################
project(
        DetachedLightning
        VERSION 1.3.0
        DESCRIPTION "A plugin that detaches lightning spell effects from the caster's hand."
        LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# ########################################################################################################################
# ## Build options
# ########################################################################################################################
message("Options:")
option(AUTO_PLUGIN_DEPLOYMENT "Copy the build output and addons to env:${PROJECT_NAME}OutputDir." OFF)
message("\tAuto plugin deployment: ${AUTO_PLUGIN_DEPLOYMENT}")

# #######################################################################################################################
# # Add CMake features
# #######################################################################################################################
include(XSEPlugin)

# #######################################################################################################################
# # Find dependencies
# #######################################################################################################################
find_package(xbyak CONFIG REQUIRED)
find_path(CLIB_UTIL_INCLUDE_DIRS "ClibUtil/utils.hpp")

target_sources(
	${PROJECT_NAME}
	PRIVATE
	src/Config.cpp
	src/Hooks.cpp
	src/Main.cpp
)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
	${CLIB_UTIL_INCLUDE_DIRS}
	extern/CommonLibSSE-NG/include
	include/
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	xbyak::xbyak
)

# https://gitlab.kitware.com/cmake/cmake/-/issues/24922#note_1371990
if(MSVC_VERSION GREATER_EQUAL 1936 AND MSVC_IDE) # 17.6+
	# When using /std:c++latest, "Build ISO C++23 Standard Library Modules" defaults to "Yes".
	# Default to "No" instead.
	#
	# As of CMake 3.26.4, there isn't a way to control this property
	# (https://gitlab.kitware.com/cmake/cmake/-/issues/24922),
	# We'll use the MSBuild project system instead
	# (https://learn.microsoft.com/en-us/cpp/build/reference/vcxproj-file-structure)
	file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/Directory.Build.props" CONTENT [==[
<Project>
  <ItemDefinitionGroup>
    <ClCompile>
      <BuildStlModules>false</BuildStlModules>
    </ClCompile>
  </ItemDefinitionGroup>
</Project>
]==] @ONLY)
endif()

# #######################################################################################################################
# # Automatic deployment
# #######################################################################################################################
if(AUTO_PLUGIN_DEPLOYMENT)
	foreach(DEPLOY_TARGET $ENV{${PROJECT_NAME}OutputDir})
		message("Copying AIO to ${DEPLOY_TARGET}")
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "${DEPLOY_TARGET}\\SKSE\\Plugins\\$<TARGET_FILE_NAME:${PROJECT_NAME}>"
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> "${DEPLOY_TARGET}\\SKSE\\Plugins\\$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
		)
	endforeach()

	if(NOT DEFINED ENV{${PROJECT_NAME}OutputDir})
		message("When using AUTO_PLUGIN_DEPLOYMENT option, you need to set environment variable '${PROJECT_NAME}OutputDir'")
	endif()
endif()