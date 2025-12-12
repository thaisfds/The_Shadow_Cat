#pragma once
#include <map>
#include <string>
#include <functional>
#include "Components/Skills/SkillBase.h"

class SkillFactory
{
public:
	using SkillCreator = std::function<SkillBase*(Actor*)>;
	
	static SkillFactory& Instance()
	{
		static SkillFactory instance;
		return instance;
	}
	
	void RegisterSkill(const std::string& name, SkillCreator creator)
	{
		mSkillCreators[name] = creator;
	}
	
	SkillBase* CreateSkill(const std::string& skillName, Actor* owner)
	{
		auto it = mSkillCreators.find(skillName);
		if (it != mSkillCreators.end()) return it->second(owner);
		
		SDL_Log("Warning: Unknown skill '%s'", skillName.c_str());
		return nullptr;
	}

	static void InitializeSkills();
	
private:
	SkillFactory() = default;
	std::map<std::string, SkillCreator> mSkillCreators;
};