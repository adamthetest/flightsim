#pragma once
#include "Entity.h"
#include "physics/FlightPhysics.h"

class PlayerShip : public Entity {
public:
    PlayerShip();

    RigidBody    body;
    FlightParams flightParams;

    float health         = 1.0f;  // 0..1
    float shields        = 1.0f;  // 0..1
    float shieldRecharge = 0.05f; // per second when not hit
    float shieldHitTimer = 0.0f;  // cooldown before recharge starts

    float weaponCooldown   = 0.0f;
    float weaponFireRate   = 5.0f; // shots per second

    bool  boostActive      = false;

    // Called each frame to sync position/rotation from physics body.
    void SyncFromBody();
};
