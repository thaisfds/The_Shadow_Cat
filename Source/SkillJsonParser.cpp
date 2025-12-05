#include "SkillJsonParser.h"
#include <SDL.h>

float SkillJsonParser::GetFloatEffectValue(const nlohmann::json &skillData, const std::string &effectName)
{
	if (skillData.contains("effects") && skillData["effects"].is_array())
	{
		for (const auto& effect : skillData["effects"])
		{
			if (effect.contains("type") && effect["type"].get<std::string>() == effectName)
			{
				if (effect.contains("amount") && effect["amount"].is_number())
					return effect["amount"].get<float>();
			}
		}
	}

	SDL_Log("Skill data does not contain effect: %s or it is not a number", effectName.c_str());
	return 0.0f;
}

float SkillJsonParser::GetFloatValue(const nlohmann::json& skillData, const std::string& key)
{
	if (skillData.contains(key) && skillData[key].is_number())
		return skillData[key].get<float>();

	SDL_Log("Skill data does not contain key: %s or it is not a number", key.c_str());
	return 0.0f;
}

std::string SkillJsonParser::GetStringValue(const nlohmann::json& skillData, const std::string& key)
{
	if (skillData.contains(key) && skillData[key].is_string())
		return skillData[key].get<std::string>();

	SDL_Log("Skill data does not contain key: %s or it is not a string", key.c_str());
	return "";
}

Collider* SkillJsonParser::GetAreaOfEffect(const nlohmann::json& skillData)
{
	if (skillData.contains("areaOfEffect") && skillData["areaOfEffect"].is_object())
	{
		const auto& aoeData = skillData["areaOfEffect"];
		if (aoeData.contains("type") && aoeData["type"].is_string())
		{
			std::string type = aoeData["type"].get<std::string>();
			if (type == "circle" && aoeData.contains("radius") && aoeData["radius"].is_number())
			{
				float radius = aoeData["radius"].get<float>();
				return new CircleCollider(radius);
			}
			else if (type == "aabb" &&
					 aoeData.contains("width") && aoeData["width"].is_number() &&
					 aoeData.contains("height") && aoeData["height"].is_number())
			{
				float width = aoeData["width"].get<float>();
				float height = aoeData["height"].get<float>();
				return new AABBCollider(width, height);
			}
		}
	}

	SDL_Log("Skill data does not contain valid areaOfEffect");
	return nullptr;
}