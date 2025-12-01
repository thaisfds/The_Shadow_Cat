#include "SkillInputHandler.h"
#include "BasicAttack.h"
#include "ClawAttack.h"
#include "Dash.h"
#include "FurBall.h"
#include "SkillBase.h"
#include "Stomp.h"

SkillInputHandler::SkillInputHandler(Actor* owner, int updateOrder)
    : Component(owner, updateOrder)
{
    mKeyToSkill[SDL_SCANCODE_Q] = new ClawAttack(owner);
    mKeyToSkill[SDL_SCANCODE_E] = new Dash(owner);
    mKeyToSkill[SDL_SCANCODE_R] = new Stomp(owner);
    mKeyToSkill[SDL_SCANCODE_F] = new FurBall(owner);
}

void SkillInputHandler::HandleEvent(const SDL_Event& event)
{
    if (event.type != SDL_KEYDOWN) return;
    if (event.key.repeat != 0) return;
    
    SDL_Scancode pressedKey = event.key.keysym.scancode;
    
    auto it = mKeyToSkill.find(pressedKey);
    if (it == mKeyToSkill.end()) return;

    SkillBase* skill = it->second;
    if (skill && skill->CanUse())
    {
        SDL_Log("Skill used: %s", SDL_GetScancodeName(pressedKey));
        skill->Execute();
    }
}

void SkillInputHandler::AssignSkillToSlot(int slot, SkillBase* skill)
{
    SDL_Scancode key = SlotToKey(slot);
    if (key != SDL_SCANCODE_UNKNOWN) mKeyToSkill[key] = skill;
}

void SkillInputHandler::ClearSlot(int slot)
{
    SDL_Scancode key = SlotToKey(slot);
    if (key != SDL_SCANCODE_UNKNOWN) mKeyToSkill[key] = nullptr;
}

SkillBase* SkillInputHandler::GetSkillInSlot(int slot) const
{
    SDL_Scancode key = SlotToKey(slot);
    if (key == SDL_SCANCODE_UNKNOWN) return nullptr;
    
    auto it = mKeyToSkill.find(key);
    if (it != mKeyToSkill.end()) return it->second;
    
    return nullptr;
}

bool SkillInputHandler::IsSlotEmpty(int slot) const
{
    SkillBase* skill = GetSkillInSlot(slot);
    return skill == nullptr;
}

SDL_Scancode SkillInputHandler::GetKeyForSlot(int slot) const
{
    return SlotToKey(slot);
}

SDL_Scancode SkillInputHandler::SlotToKey(int slot) const
{
    switch (slot)
    {
        case 0: return SDL_SCANCODE_Q;
        case 1: return SDL_SCANCODE_E;
        case 2: return SDL_SCANCODE_R;
        case 3: return SDL_SCANCODE_F;
        default: return SDL_SCANCODE_UNKNOWN;
    }
}

int SkillInputHandler::KeyToSlot(SDL_Scancode key) const
{
    switch (key)
    {
        case SDL_SCANCODE_Q: return 0;
        case SDL_SCANCODE_E: return 1;
        case SDL_SCANCODE_R: return 2;
        case SDL_SCANCODE_F: return 3;
        default: return -1;
    }
}