#pragma once
#include "Actor.h"

class ParticleSystemComponent;

class DebugActor : public Actor {
public:
    DebugActor(Game* game);
    ~DebugActor() override = default;

    ParticleSystemComponent *GetParticleSystemComponent() { return mParticleSystem; }
private:
    ParticleSystemComponent *mParticleSystem;
};
