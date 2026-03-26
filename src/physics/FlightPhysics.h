#pragma once
#include "raylib.h"
#include "raymath.h"

struct InputState; // defined in core/InputManager.h

// Per-entity physics state.
struct RigidBody {
    Vector3 position  = {0.0f, 0.0f, 0.0f};
    Vector3 velocity  = {0.0f, 0.0f, 0.0f};
    float   pitch     = 0.0f;  // accumulated rotation, radians
    float   yaw       = 0.0f;
    float   roll      = 0.0f;
    float   mass      = 1.0f;
    float   drag      = 0.97f; // velocity multiplied by drag^dt each frame

    // Updated by Integrate(); use these for rendering and camera.
    Matrix  transform = {0};   // full model-to-world rotation matrix
    Vector3 forward   = {0.0f, 0.0f, -1.0f};
    Vector3 up        = {0.0f, 1.0f,  0.0f};
    Vector3 right     = {1.0f, 0.0f,  0.0f};

    float   speed() const; // magnitude of velocity
};

// Tuning knobs per ship type.
struct FlightParams {
    float thrustForce     = 60.0f;
    float pitchRate       = 1.6f;  // rad/s
    float yawRate         = 1.2f;  // rad/s
    float rollRate        = 2.5f;  // rad/s
    float maxSpeed        = 120.0f;
    float boostMultiplier = 2.2f;
    float brakeDrag       = 0.90f; // drag when no thrust input
};

class FlightPhysics {
public:
    // Stateless: integrates body in-place given player input and dt.
    static void Integrate(RigidBody& body, const InputState& input,
                          const FlightParams& params, float dt);

    // Enemy AI variant: fly toward target position.
    static void IntegrateToward(RigidBody& body, Vector3 target,
                                const FlightParams& params, float dt,
                                float aggressionSpeed = 1.0f);
};
