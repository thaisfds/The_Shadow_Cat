#pragma once
#include "../Component.h"
#include <string>

class Character;

class SkillBase : public Component
{
public:
    SkillBase(class Actor* owner, int updateOrder = 100);
    virtual ~SkillBase() = default;

    void Update(float deltaTime) override;
    virtual void ProcessInput(const uint8_t* keyState) override;
    
    virtual void Execute() = 0;
    virtual bool CanUse() const;
    
    void StartCooldown() { mCurrentCooldown = mCooldown; }
    float GetCooldown() const { return mCurrentCooldown; }
    bool IsOnCooldown() const { return mCurrentCooldown > 0.0f; }
    
    const std::string& GetName() const { return mName; }
    const std::string& GetDescription() const { return mDescription; }

protected:
    class Character *mCharacter;

    std::string mName;
    std::string mDescription;
    float mCooldown;
    float mCurrentCooldown;
};