#include "Hooks.h"

using namespace DetachedLightning;

REL::Relocation<decltype(BeamProjectileHook::m_beamProjectileConstructor)>& BeamProjectileHook::m_getBeamProjectileConstructor() {
  static REL::Relocation<decltype(m_beamProjectileConstructor)> value(RELOCATION_ID(42928, 43746));
  SKSE::log::debug("BeamProjectileHook: Will try to hook into {}", value.address());
  return value;
}

void BeamProjectileHook::Hook(SKSE::Trampoline& trampoline) {
  SKSE::log::debug("Starting to hook BeamProjectile::Constructor");
  BeamProjectileHook::m_originalBeamProjectileConstructor
    = trampoline.write_call<5>(BeamProjectileHook::m_getBeamProjectileConstructor().address(), reinterpret_cast<uintptr_t>(BeamProjectileHook::m_beamProjectileConstructor));
  SKSE::log::debug("BeamProjectile constructor hook written");
}

RE::BeamProjectile* BeamProjectileHook::m_beamProjectileConstructor(RE::BeamProjectile* proj, void* launchData) {
  proj = BeamProjectileHook::m_originalBeamProjectileConstructor(proj, launchData);

  auto spell = proj->spell;
  if (spell && spell->GetCastingType() == RE::MagicSystem::CastingType::kFireAndForget) {
    SKSE::log::info("Found lightning spell.");
  }
  return proj;
}
