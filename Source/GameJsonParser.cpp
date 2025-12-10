#include "GameJsonParser.h"
#include "Components/Physics/Collider.h"
#include "Components/Physics/Physics.h"
#include "Components/Skills/SkillBase.h"
#include <SDL.h>

float GameJsonParser::GetFloatEffectValue(const nlohmann::json &skillData, const std::string &effectName)
{
	if (skillData.contains("effects") && skillData["effects"].is_array())
	{
		for (const auto& effect : skillData["effects"])
		{
			if (effect.contains("type") && effect["type"].get<std::string>() == effectName)
			{
				if (effect.contains("value") && effect["value"].is_number())
					return effect["value"].get<float>();
			}
		}
	}

	SDL_Log("Skill data does not contain effect: %s or it is not a number", effectName.c_str());
	return 0.0f;
}

float GameJsonParser::GetFloatValue(const nlohmann::json& skillData, const std::string& key)
{
	if (skillData.contains(key))
		return ExtractFloatValue(skillData, skillData[key]);

	SDL_Log("Skill data does not contain key: %s or it is not a number", key.c_str());
	return 0.0f;
}

int GameJsonParser::GetIntValue(const nlohmann::json& skillData, const std::string& key)
{
	if (skillData.contains(key) && skillData[key].is_number_integer())
		return skillData[key].get<int>();

	SDL_Log("Skill data does not contain key: %s or it is not an integer", key.c_str());
	return 0;
}

float GameJsonParser::ResolveReference(const nlohmann::json& skillData, const std::string& reference)
{
	// Handle nested paths with dot notation (e.g., "effects.range")
	size_t dotPos = reference.find('.');
	if (dotPos != std::string::npos)
	{
		std::string parent = reference.substr(0, dotPos);
		std::string child = reference.substr(dotPos + 1);
		if (skillData.contains(parent) && skillData[parent].is_object())
			return ResolveReference(skillData[parent], child);
	}
	
	// Direct field lookup
	if (skillData.contains(reference) && skillData[reference].is_number())
		return skillData[reference].get<float>();
	
	SDL_Log("Could not resolve reference: @%s", reference.c_str());
	return 0.0f;
}

float GameJsonParser::ExtractFloatValue(const nlohmann::json& skillData, const nlohmann::json& value)
{
	if (value.is_number())
		return value.get<float>();
	else if (value.is_string())
	{
		std::string str = value.get<std::string>();
		if (!str.empty() && str[0] == '@')
		{
			std::string refKey = str.substr(1); // Remove '@'
			return ResolveReference(skillData, refKey);
		}
	}
	SDL_Log("Expected float is neither a number nor a valid reference");
	return 0.0f;
}

std::string GameJsonParser::GetStringValue(const nlohmann::json& skillData, const std::string& key)
{
	if (skillData.contains(key) && skillData[key].is_string())
		return skillData[key].get<std::string>();

	SDL_Log("Skill data does not contain key: %s or it is not a string", key.c_str());
	return "";
}

std::vector<std::string> GameJsonParser::GetStringArrayValue(const nlohmann::json& skillData, const std::string& key)
{
	std::vector<std::string> result;
	if (skillData.contains(key) && skillData[key].is_array())
	{
		for (const auto& item : skillData[key])
			if (item.is_string()) result.push_back(item.get<std::string>());
		return result;
	}

	SDL_Log("Skill data does not contain key: %s or it is not an array of strings", key.c_str());
	return result;
}

Collider* GameJsonParser::GetAreaOfEffect(const nlohmann::json& skillData)
{
	if (skillData.contains("areaOfEffect") && skillData["areaOfEffect"].is_object())
	{
		const auto& aoeData = skillData["areaOfEffect"];
		if (aoeData.contains("type") && aoeData["type"].is_string())
		{
			std::string type = aoeData["type"].get<std::string>();
			if (type == "circle" && aoeData.contains("radius"))
			{
				float radius = ExtractFloatValue(skillData, aoeData["radius"]);
				return new CircleCollider(radius);
			}
			else if (type == "cone" && aoeData.contains("length") && aoeData.contains("angle"))
			{
				float length = ExtractFloatValue(skillData, aoeData["length"]);
				float angle = ExtractFloatValue(skillData, aoeData["angle"]);

				// Make a polygon collider representing the cone
				PolygonCollider* coneCollider = new PolygonCollider(
					Physics::GetConeVertices(Vector2::Zero, Vector2(1.0f, 0.0f), Math::ToRadians(angle), length)
				);
				return coneCollider;
			}
			else if (type == "aabb" 
				&& aoeData.contains("width") && aoeData["width"].is_number()
				&& aoeData.contains("height") && aoeData["height"].is_number())
			{
				float width = ExtractFloatValue(skillData, aoeData["width"]);
				float height = ExtractFloatValue(skillData, aoeData["height"]);
				return new AABBCollider(width, height);
			}
		}
	}

	SDL_Log("Skill data does not contain valid areaOfEffect");
	return nullptr;
}

UpgradeInfo GameJsonParser::GetUpgradeInfo(const nlohmann::json& skillData, const std::string& upgradeType, float *upgradeTarget)
{
	UpgradeInfo info;
	if (skillData.contains("upgrades") && skillData["upgrades"].is_array())
	{
		for (const auto& upgrade : skillData["upgrades"])
		{
			if (upgrade.contains("type") && upgrade["type"].get<std::string>() == upgradeType)
			{
				info.type = upgradeType;
				info.value = ExtractFloatValue(skillData, upgrade["value"]);
				if (upgrade.contains("maxLevel") && upgrade["maxLevel"].is_number())
					info.maxLevel = upgrade["maxLevel"].get<int>();
				return info;
			}
		}
		info.upgradeTarget = upgradeTarget;
	}

	SDL_Log("Skill data does not contain upgrade of type: %s", upgradeType.c_str());
	return info;
}