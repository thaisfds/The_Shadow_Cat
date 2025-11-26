#pragma once
#include "../Component.h"
#include <unordered_map>
#include <SDL.h>

class SkillInputHandler : public Component
{
public:
    SkillInputHandler(class Actor* owner, int updateOrder = 10);
    
    void HandleEvent(const SDL_Event& event);
    
    void AssignSkillToSlot(int slot, class SkillBase* skill);
    void ClearSlot(int slot);
    
    SkillBase* GetSkillInSlot(int slot) const;
    bool IsSlotEmpty(int slot) const;
    int GetSlotCount() const { return 4; }
    
    SDL_Scancode GetKeyForSlot(int slot) const;
    
private:
    std::unordered_map<SDL_Scancode, SkillBase*> mKeyToSkill;
    
    SDL_Scancode SlotToKey(int slot) const;
    int KeyToSlot(SDL_Scancode key) const;
};