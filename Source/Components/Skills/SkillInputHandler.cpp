#include "SkillInputHandler.h"
#include "BasicAttack.h"
#include "ClawAttack.h"
#include "Dash.h"
#include "FurBall.h"
#include "ShadowForm.h"
#include "SkillBase.h"
#include "Stomp.h"
#include "../../Game.h"

SkillInputHandler::SkillInputHandler(Actor* owner, int updateOrder)
    : Component(owner, updateOrder)
{
    mSkillFilter.belongsTo = CollisionFilter::GroupMask({CollisionGroup::PlayerSkills});
    mSkillFilter.collidesWith = CollisionFilter::GroupMask({CollisionGroup::Enemy});

    SkillInput leftMouseInput{InputType::Mouse, SDL_BUTTON_LEFT};
    SkillInput rightMouseInput{InputType::Mouse, SDL_BUTTON_RIGHT};
    SkillInput keyQInput{InputType::Keyboard, SDL_SCANCODE_Q};
    SkillInput keyEInput{InputType::Keyboard, SDL_SCANCODE_E};
    SkillInput keyShiftInput{InputType::Keyboard, SDL_SCANCODE_LSHIFT};

    mKeyToSkill[leftMouseInput] = new BasicAttack(owner);
    mKeyToSkill[rightMouseInput] = new ClawAttack(owner);
    mKeyToSkill[keyEInput] = new FurBall(owner);
    mKeyToSkill[keyQInput] = new Stomp(owner);
    mKeyToSkill[keyShiftInput] = new Dash(owner);
}

void SkillInputHandler::HandleEvent(const SDL_Event& event)
{
    if (event.type != SDL_KEYDOWN && event.type != SDL_MOUSEBUTTONDOWN) return;
    if (event.key.repeat != 0) return;

    SkillBase* skill = event.type == SDL_KEYDOWN ?
        mKeyToSkill[SkillInput{InputType::Keyboard, event.key.keysym.scancode}] :
        mKeyToSkill[SkillInput{InputType::Mouse, static_cast<Uint8>(event.button.button)}];
    
    Vector2 targetPosition = mOwner->GetGame()->GetMouseWorldPosition();
    if (skill && skill->CanUse(targetPosition, true))
    {
        SDL_Log("Skill used: %s", skill->GetName().c_str());
        skill->StartSkill(targetPosition);
    }
}

void SkillInputHandler::AssignSkillToSlot(int slot, SkillBase* skill)
{
    SkillInput key = SlotToKey(slot);
    if (key != SkillInput()) mKeyToSkill[key] = skill;
}

void SkillInputHandler::ClearSlot(int slot)
{
    SkillInput key = SlotToKey(slot);
    if (key != SkillInput()) mKeyToSkill[key] = nullptr;
}

SkillBase* SkillInputHandler::GetSkillInSlot(int slot) const
{
    SkillInput key = SlotToKey(slot);
    if (key == SkillInput()) return nullptr;
    
    auto it = mKeyToSkill.find(key);
    if (it != mKeyToSkill.end()) return it->second;
    
    return nullptr;
}

bool SkillInputHandler::IsSlotEmpty(int slot) const
{
    SkillBase* skill = GetSkillInSlot(slot);
    return skill == nullptr;
}

SkillInput SkillInputHandler::GetKeyForSlot(int slot) const
{
    return SlotToKey(slot);
}

SkillInput SkillInputHandler::SlotToKey(int slot) const
{
    if (slot == 0) return {InputType::Mouse, SDL_BUTTON_LEFT};
    else if (slot == 1) return {InputType::Mouse, SDL_BUTTON_RIGHT};
    else if (slot == 2) return {InputType::Keyboard, SDL_SCANCODE_Q};
    else if (slot == 3) return {InputType::Keyboard, SDL_SCANCODE_E};
    else if (slot == 4) return {InputType::Keyboard, SDL_SCANCODE_LSHIFT};
    else return {InputType::Keyboard, SDL_SCANCODE_UNKNOWN};
}

int SkillInputHandler::KeyToSlot(SkillInput key) const
{
    if (key == SkillInput(InputType::Mouse, SDL_BUTTON_LEFT)) return 0;
    if (key == SkillInput(InputType::Mouse, SDL_BUTTON_RIGHT)) return 1;
    if (key == SkillInput(InputType::Keyboard, SDL_SCANCODE_Q)) return 2;
    if (key == SkillInput(InputType::Keyboard, SDL_SCANCODE_E)) return 3;
    if (key == SkillInput(InputType::Keyboard, SDL_SCANCODE_LSHIFT)) return 4;
    return -1;
}

std::vector<SkillBase*> SkillInputHandler::GetAssignedSkills() const
{
    std::vector<SkillBase*> skills;
    for (const auto& pair : mKeyToSkill)
        if (pair.second) skills.push_back(pair.second);
    return skills;
}