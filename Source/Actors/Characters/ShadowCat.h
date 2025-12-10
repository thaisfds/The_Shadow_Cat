//
// Created by thais on 11/5/2025.
//

#pragma once
#include "Character.h"
#include "../../UI/Screens/HUD.h"
#include <vector>

class ShadowCat : public Character
{
public:
    explicit ShadowCat(Game *game, float forwardSpeed = 200.0f);

    void OnProcessInput(const Uint8 *keyState) override;
    void OnHandleEvent(const SDL_Event& event) override;
    void OnUpdate(float deltaTime) override;

    void TakeDamage(int damage) override;
    void Kill() override;

    void ResetCollisionFilter() const override;

    std::vector<UpgradeInfo> GetRandomUpgrades() const;
    int GetUpgradePoints() const { return mUpgradePoints; }
    void AddUpgradePoint() { mUpgradePoints++; }
    void SpendUpgradePoint() { if (mUpgradePoints > 0) mUpgradePoints--; }

private:
    class SkillInputHandler *mSkillInputHandler;
    int mUpgradePoints = 0;
};