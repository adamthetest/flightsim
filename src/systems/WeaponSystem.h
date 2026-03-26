#pragma once
#include "entities/Entity.h"  // EntityId

class World;
struct InputState;

class WeaponSystem {
public:
    static constexpr float PROJECTILE_SPEED = 220.0f;

    // Handles player fire input and enemy fire decisions.
    // Spawns Projectile entities into world.
    static void Update(World& world, const InputState& input, float dt);

private:
    static void FireFrom(World& world, const class RigidBody& body,
                         float damage, EntityId ownerId, bool fromPlayer);
};
