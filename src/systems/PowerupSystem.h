#pragma once

class World;
class AudioManager;

class PowerupSystem {
public:
    static constexpr float PICKUP_RADIUS = 4.0f;

    // Checks proximity between player and powerups.
    // Applies effects to world.resources and deactivates collected powerups.
    static void Update(World& world, AudioManager& audio);
};
