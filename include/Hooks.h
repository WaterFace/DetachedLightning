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

  class TESObjectREFR_SetPositionHook {
  public:
    static void Hook(SKSE::Trampoline& trampoline);
  private:
    static void m_SetPosition(RE::BeamProjectile* proj, RE::NiPoint3* pos);
    static REL::Relocation<decltype(m_SetPosition)>& m_getSetPosition();

    static inline REL::Relocation<decltype(m_SetPosition)> m_originalUpdatePos;
  };
  class TESObjectREFR_SetRotationXHook {
  public:
    static void Hook(SKSE::Trampoline& trampoline);
  private:
    static void m_SetRotationX(RE::BeamProjectile* proj, RE::NiPoint3* pos);
    static REL::Relocation<decltype(m_SetRotationX)>& m_getSetRotationX();

    static inline REL::Relocation<decltype(m_SetRotationX)> m_originalUpdatePos;
  };
  class TESObjectREFR_SetRotationZHook {
  public:
    static void Hook(SKSE::Trampoline& trampoline);
  private:
    static void m_SetRotationZ(RE::BeamProjectile* proj, RE::NiPoint3* pos);
    static REL::Relocation<decltype(m_SetRotationZ)>& m_getSetRotationZ();

    static inline REL::Relocation<decltype(m_SetRotationZ)> m_originalUpdatePos;
  };

}