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

template<typename T>
T GameJsonParser::GetValue(const nlohmann::json& data, const std::string& key)
{
	// Check if key contains dot notation (nested path)
	if (key.find('.') != std::string::npos)
		return ResolveReference<T>(data, key);
	
	// Direct key lookup
	if (data.contains(key))
		return ExtractValue<T>(data, data[key]);

	SDL_Log("Data does not contain key: %s", key.c_str());
	return T{};
}

template<typename T>
T GameJsonParser::GetValue(const nlohmann::json& data, const std::string& key, const T& defaultValue)
{
	// Check if key contains dot notation (nested path)
	if (key.find('.') != std::string::npos)
	{
		// For nested paths, need to check if path exists
		try { return ResolveReference<T>(data, key); }
		catch(...) { return defaultValue; }
	}
	
	// Direct key lookup
	if (data.contains(key))
		return ExtractValue<T>(data, data[key]);
	
	return defaultValue;
}

float GameJsonParser::GetFloatValue(const nlohmann::json& skillData, const std::string& key)
{
	if (skillData.contains(key))
		return ExtractValue<float>(skillData, skillData[key]);

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

template<typename T>
T GameJsonParser::ResolveReference(const nlohmann::json& data, const std::string& reference)
{
	// Handle nested paths with dot notation (e.g., "effects.range")
	size_t dotPos = reference.find('.');
	if (dotPos != std::string::npos)
	{
		std::string parent = reference.substr(0, dotPos);
		std::string child = reference.substr(dotPos + 1);
		if (data.contains(parent) && data[parent].is_object())
			return ResolveReference<T>(data[parent], child);
	}
	
	// Direct field lookup
	if (data.contains(reference))
		return data[reference].get<T>();
	
	SDL_Log("Could not resolve reference: @%s", reference.c_str());
	return T{};
}

template<typename T>
T GameJsonParser::ExtractValue(const nlohmann::json& data, const nlohmann::json& value)
{
	// Check for string references first (before trying to convert)
	if (value.is_string())
	{
		std::string str = value.get<std::string>();
		if (!str.empty() && str[0] == '@')
		{
			std::string refKey = str.substr(1); // Remove '@'
			return ResolveReference<T>(data, refKey);
		}
	}
	
	// Direct value extraction
	if (value.is_number() || value.is_string() || value.is_boolean())
		return value.get<T>();
	
	SDL_Log("Expected value is neither a valid type nor a valid reference");
	return T{};
}

// Explicit template instantiations
template float GameJsonParser::ResolveReference<float>(const nlohmann::json&, const std::string&);
template std::string GameJsonParser::ResolveReference<std::string>(const nlohmann::json&, const std::string&);
template int GameJsonParser::ResolveReference<int>(const nlohmann::json&, const std::string&);

template float GameJsonParser::ExtractValue<float>(const nlohmann::json&, const nlohmann::json&);
template std::string GameJsonParser::ExtractValue<std::string>(const nlohmann::json&, const nlohmann::json&);
template int GameJsonParser::ExtractValue<int>(const nlohmann::json&, const nlohmann::json&);

template float GameJsonParser::GetValue<float>(const nlohmann::json&, const std::string&);
template std::string GameJsonParser::GetValue<std::string>(const nlohmann::json&, const std::string&);
template int GameJsonParser::GetValue<int>(const nlohmann::json&, const std::string&);

template float GameJsonParser::GetValue<float>(const nlohmann::json&, const std::string&, const float&);
template std::string GameJsonParser::GetValue<std::string>(const nlohmann::json&, const std::string&, const std::string&);
template int GameJsonParser::GetValue<int>(const nlohmann::json&, const std::string&, const int&);

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
				float radius = ExtractValue<float>(skillData, aoeData["radius"]);
				return new CircleCollider(radius);
			}
			else if (type == "cone" && aoeData.contains("length") && aoeData.contains("angle"))
			{
				float length = ExtractValue<float>(skillData, aoeData["length"]);
				float angle = ExtractValue<float>(skillData, aoeData["angle"]);

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
				float width = ExtractValue<float>(skillData, aoeData["width"]);
				float height = ExtractValue<float>(skillData, aoeData["height"]);
				return new AABBCollider(width, height);
			}
		}
	}

	SDL_Log("Skill data does not contain valid areaOfEffect");
	return nullptr;
}

UpgradeInfo GameJsonParser::GetUpgradeInfo(SkillBase *skill, const nlohmann::json& skillData, const std::string& upgradeType, float *upgradeTarget)
{
	UpgradeInfo info;
	if (skillData.contains("upgrades") && skillData["upgrades"].is_array())
	{
		info.skill = skill;
		info.upgradeTarget = upgradeTarget;
		for (const auto& upgrade : skillData["upgrades"])
		{
			if (upgrade.contains("type") && upgrade["type"].get<std::string>() == upgradeType)
			{
				info.type = upgradeType;
				info.value = ExtractValue<float>(skillData, upgrade["value"]);
				if (upgrade.contains("maxLevel") && upgrade["maxLevel"].is_number())
					info.maxLevel = upgrade["maxLevel"].get<int>();
				return info;
			}
		}
	}

	SDL_Log("Skill data does not contain upgrade of type: %s", upgradeType.c_str());
	return info;
}