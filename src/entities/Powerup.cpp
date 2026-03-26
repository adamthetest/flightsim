#include "entities/Powerup.h"
#include <cmath>

Powerup::Powerup(PowerupType t) : Entity(EntityType::POWERUP), powerupType(t) {
    modelKey    = "assets/models/props/powerup_neutrino.glb";
    boundRadius = 1.5f;
    value       = 0.35f;
}

void Powerup::Update(float dt) {
    bobTimer += dt;
    position.y = baseY + sinf(bobTimer * 1.5f) * 0.8f;
    // Slow spin on Y axis via rotation matrix
    rotation = MatrixRotateY(bobTimer * 0.8f);
}
