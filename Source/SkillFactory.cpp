#include "SkillFactory.h"
#include "Components/Skills/BasicAttack.h"
#include "Components/Skills/ClawAttack.h"
#include "Components/Skills/Dash.h"
#include "Components/Skills/ShadowForm.h"
#include "Components/Skills/Stomp.h"
#include "Components/Skills/FurBall.h"
#include "Components/Skills/WhiteSlash.h"
#include "Components/Skills/WhiteBomb.h"
#include "Components/Skills/WhiteBubble.h"
#include "Components/Skills/BossHealing.h"
#include "Actors/Actor.h"

void SkillFactory::InitializeSkills()
{
	SkillFactory::Instance().RegisterSkill("basicAttackSkill", [](Actor *owner) { return new BasicAttack(owner); });
	SkillFactory::Instance().RegisterSkill("clawAttackSkill", [](Actor *owner){ return new ClawAttack(owner); });
	SkillFactory::Instance().RegisterSkill("dashSkill", [](Actor *owner) { return new Dash(owner); });
	SkillFactory::Instance().RegisterSkill("shadowFormSkill", [](Actor *owner) { return new ShadowForm(owner); });
	SkillFactory::Instance().RegisterSkill("stompSkill", [](Actor *owner) { return new Stomp(owner); });
	SkillFactory::Instance().RegisterSkill("furBallSkill", [](Actor *owner) { return new FurBall(owner); });
	SkillFactory::Instance().RegisterSkill("whiteSlashSkill", [](Actor *owner) { return new WhiteSlash(owner); });
	SkillFactory::Instance().RegisterSkill("whiteBombSkill", [](Actor *owner) { return new WhiteBomb(owner); });
	SkillFactory::Instance().RegisterSkill("whiteBubbleSkill", [](Actor *owner) { return new WhiteBubble(owner); });
	SkillFactory::Instance().RegisterSkill("bossHealingSkill", [](Actor *owner) { return new BossHealing(owner); });
}