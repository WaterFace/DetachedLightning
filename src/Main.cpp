#define DLLEXPORT __declspec(dllexport)

#include <stddef.h>
#include "Config.h"
#include "Hooks.h"
#include "Plugin.h"

using namespace DetachedLightning;
using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

void InitializeLogging()
{
  auto path = log_directory();
  if (!path)
  {
    report_and_fail("Unable to lookup SKSE logs directory.");
  }
  *path /= PluginDeclaration::GetSingleton()->GetName();
  *path += L".log";

  std::shared_ptr<spdlog::logger> log;
  if (IsDebuggerPresent())
  {
    log = std::make_shared<spdlog::logger>(
        "Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
  }
  else
  {
    log = std::make_shared<spdlog::logger>(
        "Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
  }
  const auto &debugConfig = DetachedLightning::Config::GetSingleton().GetDebug();
  log->set_level(debugConfig.GetLogLevel());
  log->flush_on(debugConfig.GetFlushLevel());

  spdlog::set_default_logger(std::move(log));
  spdlog::set_pattern("[%H:%M:%S] [%l] [%s:%#] %v");
}

void InitializeHooking()
{
  log::trace("Initializing trampoline...");
  auto &trampoline = GetTrampoline();
  trampoline.create(128);
  log::trace("Trampoline initialized.");

  BeamProjectileHook::Hook(trampoline);
  TESObjectREFR_SetPositionHook::Hook(trampoline);
  TESObjectREFR_SetRotationXHook::Hook(trampoline);
  TESObjectREFR_SetRotationZHook::Hook(trampoline);
  RefHandle_GetHook::Hook(trampoline);
  NodeHook::Hook(trampoline);
}

void InitializeMessaging()
{
  if (!GetMessagingInterface()->RegisterListener([](MessagingInterface::Message *message)
                                                 {
      switch (message->type) {
      case MessagingInterface::kDataLoaded:
        InitializeHooking();
        break;
      } }))
  {
    stl::report_and_fail("Unable to register message listener.");
  }
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface *a_skse)
{
  InitializeLogging();

  auto *plugin = PluginDeclaration::GetSingleton();
  auto version = plugin->GetVersion();
  log::info("{} {} is loading...", plugin->GetName(), version);

  Init(a_skse);
  InitializeMessaging();

  log::info("{} has finished loading.", plugin->GetName());
  return true;
}

extern "C" DLLEXPORT auto SKSEPlugin_Version = []() noexcept
{
  SKSE::PluginVersionData v;
  v.PluginName(Plugin::NAME.data());
  v.PluginVersion(Plugin::VERSION);
  v.UsesAddressLibrary();
  v.UsesUpdatedStructs();
  return v;
}();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface *, SKSE::PluginInfo *pluginInfo)
{
  pluginInfo->name = SKSEPlugin_Version.pluginName;
  pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
  pluginInfo->version = SKSEPlugin_Version.pluginVersion;
  return true;
}
