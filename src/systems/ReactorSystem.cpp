#include "systems/ReactorSystem.h"
#include "world/World.h"
#include "entities/PlayerShip.h"
#include "raymath.h"

bool ReactorSystem::Update(ResourceBundle& resources, const PlayerShip& player,
                           bool weaponActive, float dt) {
    float drain = BASE_DRAIN_RATE;
    if (weaponActive)            drain += WEAPON_DRAIN_BONUS;
    if (player.shields > 0.0f)  drain += SHIELD_DRAIN_BONUS;

    resources.reactorFuel -= drain * dt;
    if (resources.reactorFuel < 0.0f) {
        resources.reactorFuel = 0.0f;
        return true; // reactor dead
    }
    return false;
}
