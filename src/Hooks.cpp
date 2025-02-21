#include "Hooks.h"
#include <REL/Relocation.h>
#include <xbyak/xbyak.h>

using namespace DetachedLightning;

REL::Relocation<decltype(BeamProjectileHook::m_beamProjectileConstructor)> &BeamProjectileHook::m_getBeamProjectileConstructor()
{
  // SE: 0x14074b170+0x185, in the function CreateProjectile_14074b170
  // AE: 0x140779220+0x186, in the function CreateProjectile_140779220

  static REL::Relocation<decltype(m_beamProjectileConstructor)> value(RELOCATION_ID(42928, 44108), RELOCATION_OFFSET(0x185, 0x186, 0x185));
  return value;
}

void BeamProjectileHook::Hook(SKSE::Trampoline &trampoline)
{
  SKSE::log::debug("Starting to hook BeamProjectile::Constructor");
  BeamProjectileHook::m_originalBeamProjectileConstructor = trampoline.write_call<5>(BeamProjectileHook::m_getBeamProjectileConstructor().address(), reinterpret_cast<uintptr_t>(BeamProjectileHook::m_beamProjectileConstructor));
  SKSE::log::debug("BeamProjectile constructor hook written");
}

void BeamProjectileHook::TagProjectile(RE::Projectile *proj, uint32_t tag)
{
  // Stick some data in an unused bit of memory in the Projectile struct
  proj->GetProjectileRuntimeData().pad164 = tag;
}

uint32_t BeamProjectileHook::GetTag(RE::Projectile *proj)
{
  return proj->GetProjectileRuntimeData().pad164;
}

RE::BeamProjectile *BeamProjectileHook::m_beamProjectileConstructor(RE::BeamProjectile *proj, void *launchData)
{
  proj = BeamProjectileHook::m_originalBeamProjectileConstructor(proj, launchData);
  auto spell = proj->GetProjectileRuntimeData().spell;

  // tag fire-and-forget beam spells (mostly lightning bolt spells)
  if (spell)
  {
    if (spell->GetCastingType() == RE::MagicSystem::CastingType::kFireAndForget)
    {
      BeamProjectileHook::TagProjectile(proj, 1);
    }
    else
    {
      BeamProjectileHook::TagProjectile(proj, 0);
    }
  }
  return proj;
}

REL::Relocation<decltype(TESObjectREFR_SetPositionHook::m_SetPosition)> &TESObjectREFR_SetPositionHook::m_getSetPosition()
{
  // SE: 0x140733cc0+0x2db, in the function BeamProjectile__UpdateImpl_140733cc0
  // AE: 0x14075ffa0+0x2d7, in the function BeamProjectile__UpdateImpl_14075ffa0
  static REL::Relocation<decltype(m_SetPosition)> value(RELOCATION_ID(42586, 43749), RELOCATION_OFFSET(0x2db, 0x2d7, 0x312));
  return value;
}

void TESObjectREFR_SetPositionHook::Hook(SKSE::Trampoline &trampoline)
{
  SKSE::log::debug("Starting to hook TESObjectREFR::SetPosition");
  TESObjectREFR_SetPositionHook::m_originalSetPosition = trampoline.write_call<5>(TESObjectREFR_SetPositionHook::m_getSetPosition().address(), reinterpret_cast<uintptr_t>(TESObjectREFR_SetPositionHook::m_SetPosition));
  SKSE::log::debug("TESObjectREFR::SetPosition hook written");
}

void TESObjectREFR_SetPositionHook::m_SetPosition(RE::BeamProjectile *proj, RE::NiPoint3 *pos)
{
  // Just don't update the position of projectiles tagged as a lightning bolt-type spell
  if (proj && BeamProjectileHook::GetTag(proj) != 1)
  {
    m_originalSetPosition(proj, pos);
  }
}

REL::Relocation<decltype(RefHandle_GetHook::m_refHandle_Get)> &RefHandle_GetHook::m_getRefHandle_Get()
{
  // SE: 0x14074b170+0x117, in the function CreateProjectile_14074b170
  // AE: 0x140779220+0x118, in the function CreateProjectile_140779220
  static REL::Relocation<decltype(m_refHandle_Get)> value(RELOCATION_ID(42928, 44108), RELOCATION_OFFSET(0x117, 0x118, 0x117));
  return value;
}

void RefHandle_GetHook::Hook(SKSE::Trampoline &trampoline)
{
  SKSE::log::debug("Starting to hook RefHandle_GetHook::RefHandle::Get");

  RefHandle_GetHook::m_originalRefHandle_Get = trampoline.write_call<5>(
      RefHandle_GetHook::m_getRefHandle_Get().address(),
      reinterpret_cast<uintptr_t>(RefHandle_GetHook::m_refHandle_Get));

  SKSE::log::debug("RefHandle_GetHook::RefHandle::Get hook written");
}

bool RefHandle_GetHook::m_refHandle_Get(uint32_t *handle, RE::Projectile **proj)
{
  // I think this just lies to the game so it doesn't attach the newly created lightning
  // spell to the caster's hand

  auto found = m_originalRefHandle_Get(handle, proj);
  if (!found || !*proj)
  {
    return found;
  }

  return (BeamProjectileHook::GetTag(*proj) != 1);
}

void NodeHook::Hook(SKSE::Trampoline &trampoline)
{
  // This is a weird one. The original code seems to write to the node's x,y,z coordinates,
  // but we want to make that conditional. So we'll insert some asm code that calls into
  // our function which updates the node's position conditionally, and leave behind a jmp
  // to where we actually want to return to, skipping the unconditional coordinate update.

  // This is directly from Fenix's original code:
  // https://github.com/fenix31415/FenixProjectilesAPI/blob/2d41c89e43a49ffdd0c92110d8808acf590fc112/src/Hooks.h#L70
  // which is licensed under the MIT license

  SKSE::log::debug("Trying to hook into the middle of BeamProjectile__UpdateImpl.");

  uintptr_t return_addr = RELOCATION_ID(42586, 43749).address() + RELOCATION_OFFSET(0x2d3, 0x2cf, 0x30a);

  struct Code : Xbyak::CodeGenerator
  {
    Code(uintptr_t func_addr, uintptr_t ret_addr)
    {
      Xbyak::Label nocancel;

      // rsi  = proj
      // xmm0 -- xmm2 = node pos
      mov(r9, rsi);        // copy the projectile to a register so it can be read by our function
      mov(rax, func_addr); // put our function's address into rax
      call(rax);           // call our function
      mov(rax, ret_addr);  // put our desired return location into rax
      jmp(rax);            // return to where we specified
    }
  } xbyakCode{uintptr_t(m_moveNode), return_addr};

  auto codeSize = xbyakCode.getSize();
  auto code = trampoline.allocate(codeSize);
  std::memcpy(code, xbyakCode.getCode(), codeSize);

  trampoline.write_branch<5>(RELOCATION_ID(42586, 43749).address() + RELOCATION_OFFSET(0x2c1, 0x2bd, 0x2f8), (uintptr_t)code);
  SKSE::log::debug("Hook into the middle of BeamProjectile__UpdateImpl written.");
}

void NodeHook::m_moveNode(float x, float y, float z, RE::Projectile *proj)
{
  auto node = proj->Get3D();

  if (node && BeamProjectileHook::GetTag(proj) != 1)
  {
    node->local.translate.x = x;
    node->local.translate.y = y;
    node->local.translate.z = z;
  }
}

void TESObjectREFR_SetRotationXHook::Hook(SKSE::Trampoline &trampoline)
{
  SKSE::log::debug("Trying to hook into TESObjectREFR_SetRotationXHook");

  m_originalSetRotationX = trampoline.write_call<5>(
      m_getSetRotationX().address(),
      reinterpret_cast<uintptr_t>(m_SetRotationX));

  SKSE::log::debug("Hook into TESObjectREFR_SetRotationXHook written.");
}

void TESObjectREFR_SetRotationXHook::m_SetRotationX(RE::BeamProjectile *proj, RE::NiPoint3 *pos)
{
  if (BeamProjectileHook::GetTag(proj) != 1)
  {
    m_originalSetRotationX(proj, pos);
  }
}
REL::Relocation<decltype(TESObjectREFR_SetRotationXHook::m_SetRotationX)> &TESObjectREFR_SetRotationXHook::m_getSetRotationX()
{
  // SE: 0x140733cc0+0x1ba, in the function BeamProjectile__UpdateImpl_140733cc0
  // AE: 0x14075ffa0+0x1b6, in the function BeamProjectile__UpdateImpl_14075ffa0
  static REL::Relocation<decltype(m_SetRotationX)> value(RELOCATION_ID(42586, 43749), RELOCATION_OFFSET(0x1ba, 0x1b6, 0x1ba));
  return value;
}

void TESObjectREFR_SetRotationZHook::Hook(SKSE::Trampoline &trampoline)
{
  SKSE::log::debug("Trying to hook into TESObjectREFR_SetRotationZHook");

  m_originalSetRotationZ = trampoline.write_call<5>(
      m_getSetRotationZ().address(),
      reinterpret_cast<uintptr_t>(m_SetRotationZ));

  SKSE::log::debug("Hook into TESObjectREFR_SetRotationXHook written.");
}

void TESObjectREFR_SetRotationZHook::m_SetRotationZ(RE::BeamProjectile *proj, RE::NiPoint3 *pos)
{
  if (BeamProjectileHook::GetTag(proj) != 1)
  {
    m_originalSetRotationZ(proj, pos);
  }
}
REL::Relocation<decltype(TESObjectREFR_SetRotationZHook::m_SetRotationZ)> &TESObjectREFR_SetRotationZHook::m_getSetRotationZ()
{
  // SE: 0x140733cc0+0x1cd, in the function BeamProjectile__UpdateImpl_140733cc0
  // AE: 0x14075ffa0+0x1c9, in the function BeamProjectile__UpdateImpl_14075ffa0
  static REL::Relocation<decltype(m_SetRotationZ)> value(RELOCATION_ID(42586, 43749), RELOCATION_OFFSET(0x1cd, 0x1c9, 0x1cd));
  return value;
}
