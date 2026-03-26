#pragma once
#include "raylib.h"

enum class AIState { PATROL, CHASE, ATTACK, EVADE };

struct AIContext {
    AIState state          = AIState::PATROL;
    Vector3 patrolTarget   = {0.0f, 0.0f, 0.0f};
    float   stateTimer     = 0.0f;   // time in current state
    float   attackCooldown = 0.0f;   // time until next shot
    float   attackRange    = 80.0f;
    float   chaseRange     = 200.0f;
    float   evadeThreshold = 0.15f;  // health fraction to trigger evade
    float   patrolRadius   = 60.0f;
    int     patrolIndex    = 0;
};

class RigidBody; // physics/FlightPhysics.h
class FlightParams;

class EnemyAI {
public:
    // Updates aiCtx and body given player position and player health.
    // Returns true if the AI wants to fire this frame.
    static bool Update(AIContext& ctx, RigidBody& body,
                       const FlightParams& params,
                       Vector3 playerPos, float selfHealth, float dt);

private:
    static void ChooseNewPatrolTarget(AIContext& ctx, Vector3 currentPos);
};
