
#include "DebugActor.h"
#include "../Game.h"
#include "../Components/ParticleSystemComponent.h"

DebugActor::DebugActor(Game* game)
    : Actor(game)
{
    mParticleSystem = new ParticleSystemComponent(this, 3, 3, 100, 10, false);
}