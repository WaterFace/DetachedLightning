#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace DetachedLightning {
  class BeamProjectileHook {
  public:
    static void Hook(SKSE::Trampoline& trampoline);

    static void TagProjectile(RE::Projectile* proj, uint32_t tag);
    static uint32_t GetTag(RE::Projectile* proj);
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

    static inline REL::Relocation<decltype(m_SetPosition)> m_originalSetPosition;
  };
  class TESObjectREFR_SetRotationXHook {
  public:
    static void Hook(SKSE::Trampoline& trampoline);
  private:
    static void m_SetRotationX(RE::BeamProjectile* proj, RE::NiPoint3* pos);
    static REL::Relocation<decltype(m_SetRotationX)>& m_getSetRotationX();

    static inline REL::Relocation<decltype(m_SetRotationX)> m_originalSetRotationX;
  };
  class TESObjectREFR_SetRotationZHook {
  public:
    static void Hook(SKSE::Trampoline& trampoline);
  private:
    static void m_SetRotationZ(RE::BeamProjectile* proj, RE::NiPoint3* pos);
    static REL::Relocation<decltype(m_SetRotationZ)>& m_getSetRotationZ();

    static inline REL::Relocation<decltype(m_SetRotationZ)> m_originalUpdatePos;
  };

  class RefHandle_GetHook {
  public:
    static void Hook(SKSE::Trampoline& trampoline);
  private:
    static bool m_refHandle_Get(uint32_t* handle, RE::Projectile** proj);
    static REL::Relocation<decltype(m_refHandle_Get)>& m_getRefHandle_Get();

    static inline REL::Relocation<decltype(m_refHandle_Get)> m_originalRefHandle_Get;
  };

  class NodeHook {
  public:
    static void Hook(SKSE::Trampoline& trampoline);
  private:
    static void m_moveNode(float x, float y, float z, RE::Projectile* proj);
  };

}