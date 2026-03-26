#include "rendering/ParticleSystem.h"
#include "raymath.h"
#include <cstdlib>
#include <cmath>

static float Randf() { return (float)rand() / (float)RAND_MAX; }
static float RandRange(float lo, float hi) { return lo + Randf() * (hi - lo); }

int ParticleSystem::NextFreeSlot() {
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        int idx = (m_nextSlot + i) % MAX_PARTICLES;
        if (!m_pool[idx].active) {
            m_nextSlot = (idx + 1) % MAX_PARTICLES;
            return idx;
        }
    }
    // All full: evict oldest (just use m_nextSlot)
    int idx = m_nextSlot;
    m_nextSlot = (m_nextSlot + 1) % MAX_PARTICLES;
    return idx;
}

void ParticleSystem::Emit(Vector3 pos, Vector3 vel, Color color,
                           float size, float lifetime) {
    int idx = NextFreeSlot();
    Particle& p = m_pool[idx];
    p.position = pos;
    p.velocity = vel;
    p.color    = color;
    p.size     = size;
    p.lifetime = lifetime;
    p.active   = true;
}

void ParticleSystem::EmitThruster(Vector3 pos, Vector3 shipForward,
                                   float thrust, Color color) {
    if (fabsf(thrust) < 0.05f) return;
    int count = (int)(fabsf(thrust) * 4) + 1;
    for (int i = 0; i < count; ++i) {
        Vector3 spread = {RandRange(-0.5f, 0.5f),
                          RandRange(-0.5f, 0.5f),
                          RandRange(-0.5f, 0.5f)};
        Vector3 vel = Vector3Add(
            Vector3Scale(Vector3Negate(shipForward), 8.0f * fabsf(thrust)),
            Vector3Scale(spread, 3.0f));
        Emit(pos, vel,
             {color.r, color.g, color.b, (unsigned char)(180 + rand() % 75)},
             RandRange(0.1f, 0.4f),
             RandRange(0.15f, 0.4f));
    }
}

void ParticleSystem::EmitExplosion(Vector3 pos, float radius) {
    for (int i = 0; i < 60; ++i) {
        Vector3 dir = {RandRange(-1.0f, 1.0f),
                       RandRange(-1.0f, 1.0f),
                       RandRange(-1.0f, 1.0f)};
        dir = Vector3Normalize(dir);
        float speed = RandRange(3.0f, radius * 8.0f);
        Color c = (i % 3 == 0) ? (Color){255, 200, 50, 255}
                : (i % 3 == 1) ? (Color){255, 100, 20, 255}
                :                (Color){180, 180, 180, 200};
        Emit(pos, Vector3Scale(dir, speed), c,
             RandRange(0.2f, 0.6f), RandRange(0.3f, 1.2f));
    }
}

void ParticleSystem::EmitPickup(Vector3 pos) {
    for (int i = 0; i < 20; ++i) {
        float angle = RandRange(0.0f, 2.0f * PI);
        Vector3 vel = {cosf(angle) * 3.0f, RandRange(1.0f, 5.0f), sinf(angle) * 3.0f};
        Color c = {50, 200, 255, 255};
        Emit(pos, vel, c, RandRange(0.1f, 0.3f), RandRange(0.4f, 0.8f));
    }
}

void ParticleSystem::Update(float dt) {
    for (auto& p : m_pool) {
        if (!p.active) continue;
        p.lifetime -= dt;
        if (p.lifetime <= 0.0f) { p.active = false; continue; }
        p.position = Vector3Add(p.position, Vector3Scale(p.velocity, dt));
        p.velocity = Vector3Scale(p.velocity, 0.95f); // gentle drag
        p.color.a  = (unsigned char)(255.0f * (p.lifetime / 1.0f));
        if (p.color.a < 5) p.active = false;
    }
}

void ParticleSystem::Draw(Camera3D camera) const {
    // Project each particle to screen space and draw a cheap 2D circle.
    // This avoids per-particle 3D mesh rendering (DrawSphere = ~320 tris each).
    Vector3 camFwd = Vector3Normalize(
        Vector3Subtract(camera.target, camera.position));

    for (const auto& p : m_pool) {
        if (!p.active) continue;

        // Cull particles behind the camera
        Vector3 toP = Vector3Subtract(p.position, camera.position);
        float depth = Vector3DotProduct(toP, camFwd);
        if (depth < 0.1f) continue;

        Vector2 screen = GetWorldToScreen(p.position, camera);

        // Perspective-correct radius: larger when close, smaller when far
        float radius = fmaxf(1.0f, p.size * 40.0f / depth);
        if (radius > 24.0f) radius = 24.0f;

        DrawCircleV(screen, radius, p.color);
    }
}
