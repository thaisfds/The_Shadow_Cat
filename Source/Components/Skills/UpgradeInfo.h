#pragma once

#include <functional>
#include <string>

// Forward declaration to avoid circular dependency
class SkillBase;

struct UpgradeInfo
{
    std::string type;
    float value;
    int currentLevel = 0;
    int maxLevel = -1;

    const SkillBase *skill = nullptr;
    float *upgradeTarget = nullptr;
};