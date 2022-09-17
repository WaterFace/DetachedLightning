#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace DetachedLightning {
  class BeamProjectileHook {
  public:
    static void Hook(SKSE::Trampoline& trampoline);
  private:
    static RE::BeamProjectile* m_beamProjectileConstructor(RE::BeamProjectile* proj, void* launchData);
    static REL::Relocation<decltype(m_beamProjectileConstructor)>& m_getBeamProjectileConstructor();

    static inline REL::Relocation<decltype(m_beamProjectileConstructor)> m_originalBeamProjectileConstructor;
  };

  private:


  public:

  };
}