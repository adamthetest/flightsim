#pragma once
#include <cstdint>
#include <string>
#include "raylib.h"
#include "raymath.h"

using EntityId = uint32_t;

enum class EntityType {
    PLAYER,
    ENEMY,
    PROJECTILE,
    POWERUP,
    TRADE_STATION,
    ENVIRONMENT,
};

class Entity {
public:
    explicit Entity(EntityType type);
    virtual ~Entity() = default;

    EntityId   id;
    EntityType type;
    Vector3    position    = {0.0f, 0.0f, 0.0f};
    Matrix     rotation;            // world-space orientation
    float      boundRadius = 1.0f;  // sphere collider radius
    bool       active      = true;

    // Key into AssetManager for the model to draw.
    // Empty string = don't draw.
    std::string modelKey;

private:
    static EntityId s_nextId;
};
