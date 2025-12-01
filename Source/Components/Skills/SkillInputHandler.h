#pragma once
#include "../Component.h"
#include <unordered_map>
#include <SDL.h>
#include "../Physics/CollisionFilter.h"

enum class InputType
{
    None,
    Keyboard,
    Mouse,
};

struct SkillInput
{
    InputType type;
    SDL_Scancode key;
    Uint8 mouseButton;

    SkillInput(InputType type, SDL_Scancode key) : type(type), key(key), mouseButton(0) {}
    SkillInput(InputType type, Uint8 mouseButton) : type(type), key(SDL_SCANCODE_UNKNOWN), mouseButton(mouseButton) {}
    SkillInput() : type(InputType::None), key(SDL_SCANCODE_UNKNOWN), mouseButton(0) {}

    bool operator==(const SkillInput& other) const
    {
        return type == other.type && key == other.key && mouseButton == other.mouseButton;
    }

    bool operator!=(const SkillInput& other) const
    {
        return !(*this == other);
    }
};

namespace std {
    template <>
    struct hash<SkillInput> {
        size_t operator()(const SkillInput& input) const {
            return std::hash<int>()(static_cast<int>(input.type)) ^
                   (input.type == InputType::Keyboard
                        ? std::hash<int>()(input.key)
                        : std::hash<int>()(input.mouseButton));
        }
    };
}

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
    
    SkillInput GetKeyForSlot(int slot) const;
    
private:
    std::unordered_map<SkillInput, SkillBase*> mKeyToSkill;
    
    SkillInput SlotToKey(int slot) const;
    int KeyToSlot(SkillInput key) const;

    CollisionFilter mSkillFilter;
};