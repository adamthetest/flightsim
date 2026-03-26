#pragma once
#include "Entity.h"

class Projectile : public Entity {
public:
    Projectile();

    Vector3  velocity    = {0.0f, 0.0f, 0.0f};
    float    damage      = 0.25f;
    float    lifetime    = 3.0f;   // seconds remaining
    EntityId ownerId     = 0;
    bool     fromPlayer  = false;

    void Update(float dt);
};
