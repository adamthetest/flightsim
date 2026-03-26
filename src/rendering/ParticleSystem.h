#pragma once
#include <array>
#include "raylib.h"
#include "raymath.h"

struct Particle {
    Vector3 position  = {0,0,0};
    Vector3 velocity  = {0,0,0};
    Color   color     = WHITE;
    float   size      = 0.3f;
    float   lifetime  = 0.0f;  // remaining seconds
    bool    active    = false;
};

class ParticleSystem {
public:
    static constexpr int MAX_PARTICLES = 2000;

    void Update(float dt);
    // Must be called between EndMode3D() and EndDrawing() (2D pass).
    void Draw(Camera3D camera) const;

    // Emitters
    void EmitThruster(Vector3 pos, Vector3 shipForward, float thrust, Color color);
    void EmitExplosion(Vector3 pos, float radius);
    void EmitPickup(Vector3 pos);

private:
    std::array<Particle, MAX_PARTICLES> m_pool;
    int m_nextSlot = 0;

    void Emit(Vector3 pos, Vector3 vel, Color color, float size, float lifetime);
    int  NextFreeSlot();
};
