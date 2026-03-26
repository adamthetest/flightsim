#pragma once
#include "Entity.h"
#include "physics/FlightPhysics.h"
#include "ai/EnemyAI.h"

class EnemyShip : public Entity {
public:
    EnemyShip();

    RigidBody    body;
    FlightParams flightParams;
    AIContext    ai;

    float health  = 1.0f;
    float shields = 0.5f;

    void SyncFromBody();
};
