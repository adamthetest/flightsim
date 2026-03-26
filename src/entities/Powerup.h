#pragma once
#include "Entity.h"

enum class PowerupType { NEUTRINO_CANISTER };

class Powerup : public Entity {
public:
    explicit Powerup(PowerupType t);

    PowerupType powerupType;
    float       value       = 0.35f; // how much reactor fuel to restore
    float       bobTimer    = 0.0f;  // for gentle floating animation
    float       baseY       = 0.0f;

    void Update(float dt);
};
