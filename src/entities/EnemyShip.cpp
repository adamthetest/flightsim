#include "entities/EnemyShip.h"

EnemyShip::EnemyShip() : Entity(EntityType::ENEMY) {
    modelKey    = "assets/models/ships/pirate_ship.glb";
    boundRadius = 2.5f;

    body.drag = 0.97f;
    body.mass = 1.2f;

    flightParams.thrustForce     = 50.0f;
    flightParams.pitchRate       = 1.4f;
    flightParams.yawRate         = 1.0f;
    flightParams.rollRate        = 2.0f;
    flightParams.maxSpeed        = 100.0f;
    flightParams.boostMultiplier = 1.8f;
    flightParams.brakeDrag       = 0.92f;
}

void EnemyShip::SyncFromBody() {
    position = body.position;
    rotation = body.transform;
}
