#include "entities/PlayerShip.h"

PlayerShip::PlayerShip() : Entity(EntityType::PLAYER) {
    modelKey = "assets/models/ships/player_ship.glb";
    boundRadius = 2.5f;

    body.position = {0.0f, 5.0f, 0.0f};
    body.drag = 0.97f;
    body.mass = 1.0f;

    flightParams.thrustForce     = 60.0f;
    flightParams.pitchRate       = 1.6f;
    flightParams.yawRate         = 1.2f;
    flightParams.rollRate        = 2.5f;
    flightParams.maxSpeed        = 120.0f;
    flightParams.boostMultiplier = 2.2f;
    flightParams.brakeDrag       = 0.90f;
}

void PlayerShip::SyncFromBody() {
    position = body.position;
    rotation = body.transform;
}
