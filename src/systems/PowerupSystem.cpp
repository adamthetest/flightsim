#include "systems/PowerupSystem.h"
#include "world/World.h"
#include "audio/AudioManager.h"
#include "raymath.h"

void PowerupSystem::Update(World& world, AudioManager& audio) {
    Vector3 playerPos = world.player.position;

    for (auto& pu : world.powerups) {
        if (!pu->active) continue;

        float dist = Vector3Distance(playerPos, pu->position);
        if (dist <= PICKUP_RADIUS) {
            // Apply powerup
            switch (pu->powerupType) {
                case PowerupType::NEUTRINO_CANISTER:
                    world.resources.reactorFuel =
                        Clamp(world.resources.reactorFuel + pu->value, 0.0f, 1.0f);
                    world.resources.neutrinos++;
                    break;
            }
            pu->active = false;
        }
    }
}
