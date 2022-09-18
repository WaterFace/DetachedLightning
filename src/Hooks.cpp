#include "Hooks.h"

using namespace DetachedLightning;

REL::Relocation<decltype(BeamProjectileHook::m_beamProjectileConstructor)>& BeamProjectileHook::m_getBeamProjectileConstructor() {
  // SE: 0x14074b170+0x185, in the function CreateProjectile_14074b170
  // AE: 0x140779220+0x186, in the function CreateProjectile_140779220

  static REL::Relocation<decltype(m_beamProjectileConstructor)> value(RELOCATION_ID(42928, 44108), RELOCATION_OFFSET(0x185, 0x186));
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

  // tag fire-and-forget beam spells (mostly lightning bolt spells)
  if (spell) {
    if (spell->GetCastingType() == RE::MagicSystem::CastingType::kFireAndForget) {
      BeamProjectileHook::TagProjectile(proj, 1);
    }
    else {
      BeamProjectileHook::TagProjectile(proj, 0);
    }
  }
  return proj;
}
