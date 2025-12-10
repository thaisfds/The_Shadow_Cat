#pragma once

#include "Json.h"
#include "Components/Physics/Collider.h"
#include "Components/Skills/UpgradeInfo.h"

class GameJsonParser
{
public:
	GameJsonParser() = delete;

	template<typename T>
	static T GetValue(const nlohmann::json& data, const std::string& key);
	
	template<typename T>
	static T GetValue(const nlohmann::json& data, const std::string& key, const T& defaultValue);

	static int GetIntValue(const nlohmann::json& skillData, const std::string& key);
	static float GetFloatValue(const nlohmann::json& skillData, const std::string& key);
	static std::string GetStringValue(const nlohmann::json& skillData, const std::string& key);
	static std::vector<std::string> GetStringArrayValue(const nlohmann::json& skillData, const std::string& key);

	// SkillData specific methods
	static float GetFloatEffectValue(const nlohmann::json& skillData, const std::string& effectName);
	static Collider* GetAreaOfEffect(const nlohmann::json& skillData);
	static UpgradeInfo GetUpgradeInfo(const nlohmann::json& skillData, const std::string& upgradeType, float *upgradeTarget);

private:
	template<typename T>
	static T ResolveReference(const nlohmann::json& data, const std::string& reference);

	template<typename T>
	static T ExtractValue(const nlohmann::json& data, const nlohmann::json& value);
};