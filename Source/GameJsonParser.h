#pragma once

#include "Components/Skills/SkillBase.h"
#include "Json.h"
#include "Components/Physics/Collider.h"
#include "Components/Skills/UpgradeInfo.h"

class GameJsonParser
{
public:
	GameJsonParser() = delete;

	static float GetFloatEffectValue(const nlohmann::json& skillData, const std::string& effectName);
	static float GetFloatValue(const nlohmann::json& skillData, const std::string& key);
	static std::string GetStringValue(const nlohmann::json& skillData, const std::string& key);
	static Collider* GetAreaOfEffect(const nlohmann::json& skillData);
	static UpgradeInfo GetUpgradeInfo(const SkillBase *skill, const nlohmann::json& skillData, const std::string& upgradeType, float *upgradeTarget);

private:
	static float ResolveReference(const nlohmann::json& skillData, const std::string& reference);
	static float ExtractFloatValue(const nlohmann::json& skillData, const nlohmann::json& value);
};