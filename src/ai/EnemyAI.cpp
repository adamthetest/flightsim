#include "ai/EnemyAI.h"
#include "physics/FlightPhysics.h"
#include "raymath.h"
#include <cstdlib>
#include <cmath>

static Vector3 RandomOffset(float radius) {
    float angle = (float)(rand() % 360) * DEG2RAD;
    float dist  = radius * 0.4f + (float)(rand() % (int)(radius * 0.6f));
    return {cosf(angle) * dist, (float)(rand() % 20) - 10.0f, sinf(angle) * dist};
}

void EnemyAI::ChooseNewPatrolTarget(AIContext& ctx, Vector3 currentPos) {
    Vector3 offset = RandomOffset(ctx.patrolRadius);
    ctx.patrolTarget = Vector3Add(currentPos, offset);
    ctx.stateTimer   = 0.0f;
}

bool EnemyAI::Update(AIContext& ctx, RigidBody& body, const FlightParams& params,
                     Vector3 playerPos, float selfHealth, float dt) {
    bool wantsToFire = false;

    ctx.stateTimer     += dt;
    ctx.attackCooldown -= dt;

    float distToPlayer = Vector3Distance(body.position, playerPos);

    // --- State transitions ---
    switch (ctx.state) {
        case AIState::PATROL:
            if (distToPlayer < ctx.chaseRange) {
                ctx.state      = AIState::CHASE;
                ctx.stateTimer = 0.0f;
            } else {
                // Arrived at patrol target?
                float distToTarget = Vector3Distance(body.position, ctx.patrolTarget);
                if (distToTarget < 8.0f || ctx.stateTimer > 12.0f) {
                    ChooseNewPatrolTarget(ctx, body.position);
                }
            }
            break;

        case AIState::CHASE:
            if (distToPlayer < ctx.attackRange) {
                ctx.state      = AIState::ATTACK;
                ctx.stateTimer = 0.0f;
            } else if (distToPlayer > ctx.chaseRange * 1.3f) {
                ctx.state      = AIState::PATROL;
                ctx.stateTimer = 0.0f;
                ChooseNewPatrolTarget(ctx, body.position);
            }
            if (selfHealth < ctx.evadeThreshold) {
                ctx.state      = AIState::EVADE;
                ctx.stateTimer = 0.0f;
            }
            break;

        case AIState::ATTACK:
            if (distToPlayer > ctx.attackRange * 1.5f) {
                ctx.state      = AIState::CHASE;
                ctx.stateTimer = 0.0f;
            }
            if (selfHealth < ctx.evadeThreshold) {
                ctx.state      = AIState::EVADE;
                ctx.stateTimer = 0.0f;
            }
            break;

        case AIState::EVADE:
            // Flee for 5 seconds then re-evaluate
            if (ctx.stateTimer > 5.0f) {
                ctx.state      = (distToPlayer < ctx.chaseRange) ? AIState::CHASE : AIState::PATROL;
                ctx.stateTimer = 0.0f;
            }
            break;
    }

    // --- State behaviour ---
    switch (ctx.state) {
        case AIState::PATROL:
            FlightPhysics::IntegrateToward(body, ctx.patrolTarget, params, dt, 0.6f);
            break;

        case AIState::CHASE:
            FlightPhysics::IntegrateToward(body, playerPos, params, dt, 0.9f);
            break;

        case AIState::ATTACK:
            FlightPhysics::IntegrateToward(body, playerPos, params, dt, 0.8f);
            if (ctx.attackCooldown <= 0.0f) {
                // Only fire when roughly facing the player
                Vector3 toPlayer = Vector3Normalize(Vector3Subtract(playerPos, body.position));
                float alignment  = Vector3DotProduct(body.forward, toPlayer);
                if (alignment > 0.85f) {
                    wantsToFire        = true;
                    ctx.attackCooldown = 0.8f; // fire every 0.8 s
                }
            }
            break;

        case AIState::EVADE: {
            // Flee away from player
            Vector3 awayFromPlayer = Vector3Negate(
                Vector3Normalize(Vector3Subtract(playerPos, body.position)));
            Vector3 evadeTarget = Vector3Add(body.position,
                                             Vector3Scale(awayFromPlayer, 50.0f));
            FlightPhysics::IntegrateToward(body, evadeTarget, params, dt, 1.0f);
            break;
        }
    }

    return wantsToFire;
}
