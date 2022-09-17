#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace DetachedLightning {
  class BeamProjectileHook {

  private:
    static RE::BeamProjectile* constructor(RE::BeamProjectile* proj, void* launchData) {
      proj = m_vanillaConstructor(proj, launchData);

      auto spell = proj->spell;
      if (spell && spell->GetCastingType() == RE::MagicSystem::CastingType::kFireAndForget) {
        SKSE::log::info("Found lightning spell.");
      }
      return proj;
    }

    static REL::Relocation<decltype(constructor)> m_vanillaConstructor;
  public:
    static void Hook(SKSE::Trampoline& trampoline) {
      m_vanillaConstructor = trampoline.write_call<5>(GetBeamProjectileConstructor().address(), reinterpret_cast<uintptr_t>(constructor));
      SKSE::log::debug("BeamProjectile constructor hook written");
    }

    static REL::Relocation<decltype(constructor)>& GetBeamProjectileConstructor() {
      static REL::Relocation<decltype(constructor)> value(RELOCATION_ID(42928, 43746));
      return value;
    }
  };
}