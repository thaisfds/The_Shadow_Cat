#pragma once

#include "Json.h"
#include "Components/Physics/Collider.h"

class SkillJsonParser
{
	public:
		SkillJsonParser() = delete;

	static float GetFloatEffectValue(const nlohmann::json& skillData, const std::string& effectName);
	static float GetFloatValue(const nlohmann::json& skillData, const std::string& key);
	static std::string GetStringValue(const nlohmann::json& skillData, const std::string& key);
	static Collider* GetAreaOfEffect(const nlohmann::json& skillData);

private:
	static float ResolveReference(const nlohmann::json& skillData, const std::string& reference);
	static float ExtractFloatValue(const nlohmann::json& skillData, const nlohmann::json& value);
};