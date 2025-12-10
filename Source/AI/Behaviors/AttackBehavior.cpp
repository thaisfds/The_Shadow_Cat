#include "AttackBehavior.h"
#include "../../Actors/Characters/Enemy.h"
#include "../../Actors/Characters/ShadowCat.h"

AttackBehavior::AttackBehavior()
    : AIBehavior("Attack")
{
}

void AttackBehavior::OnEnter(Enemy* enemy)
{
    // Stop moving when entering attack state
    enemy->StopMovement();
}

void AttackBehavior::Update(Enemy* enemy, float deltaTime)
{
    const ShadowCat* player = enemy->GetGame()->GetPlayer();
    if (!player) return;
    
    // Stop moving during attack
    enemy->StopMovement();
    
    // Use skill if ready
    Vector2 playerPos = player->GetPosition();
    SkillBase* skill = enemy->GetSkill();
    if (skill && skill->CanUse(playerPos))
    {
        skill->StartSkill(playerPos);
    }
}

void AttackBehavior::OnExit(Enemy* enemy)
{
    // No cleanup needed
}
