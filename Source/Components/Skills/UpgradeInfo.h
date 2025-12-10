#pragma once

#include "SkillBase.h"
#include <functional>
#include <string>

struct UpgradeInfo
{
    std::string type;
    float value;
    int currentLevel = 0;
    int maxLevel = -1;

    SkillBase *skill = nullptr;
    float *upgradeTarget = nullptr;
};