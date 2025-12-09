#pragma once

#include <functional>
#include <string>

struct UpgradeInfo
{
    std::string type;
    float value;
    int currentLevel = 0;
    int maxLevel = -1;

    float *upgradeTarget = nullptr;
};