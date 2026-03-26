#include "systems/WeaponSystem.h"
#include "world/World.h"
#include "entities/PlayerShip.h"
#include "entities/EnemyShip.h"
#include "core/InputManager.h"
#include "physics/FlightPhysics.h"
#include "raymath.h"

void WeaponSystem::FireFrom(World& world, const RigidBody& body,
                            float damage, EntityId ownerId, bool fromPlayer) {
    Vector3 vel = Vector3Scale(body.forward, PROJECTILE_SPEED);
    vel = Vector3Add(vel, body.velocity); // inherit ship velocity
    world.SpawnProjectile(body.position, vel, damage, ownerId, fromPlayer);
}

void WeaponSystem::Update(World& world, const InputState& input, float dt) {
    PlayerShip& player = world.player;

    // --- Player firing ---
    player.weaponCooldown -= dt;
    if (input.fire && player.weaponCooldown <= 0.0f) {
        FireFrom(world, player.body, 0.25f, player.id, true);
        player.weaponCooldown = 1.0f / player.weaponFireRate;
    }

    // --- Enemy firing (driven by EnemyAI, which sets wantsToFire) ---
    // AI update happens in World::Update; EnemyAI::Update returns wantsToFire.
    // WeaponSystem just provides the fire helper used from World::Update.
}
