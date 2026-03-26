#include "entities/Projectile.h"
#include "raymath.h"

Projectile::Projectile() : Entity(EntityType::PROJECTILE) {
    modelKey    = ""; // rendered as a billboard or line, not a full model
    boundRadius = 0.4f;
}

void Projectile::Update(float dt) {
    position = Vector3Add(position, Vector3Scale(velocity, dt));
    lifetime -= dt;
    if (lifetime <= 0.0f) active = false;
}
