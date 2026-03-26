#include "physics/FlightPhysics.h"
#include "core/InputManager.h"
#include <cmath>

float RigidBody::speed() const {
    return Vector3Length(velocity);
}

void FlightPhysics::Integrate(RigidBody& body, const InputState& input,
                              const FlightParams& params, float dt) {
    // --- Rotation ---
    float pitchInput = input.pitch;
    float yawInput   = input.yaw;
    float rollInput  = input.roll;

    body.pitch += pitchInput * params.pitchRate * dt;
    body.yaw   += yawInput   * params.yawRate   * dt;
    body.roll  += rollInput  * params.rollRate   * dt;

    // Build rotation matrix: yaw then pitch then roll (extrinsic ZYX = intrinsic XYZ)
    Matrix rotYaw   = MatrixRotateY(body.yaw);
    Matrix rotPitch = MatrixRotateX(body.pitch);
    Matrix rotRoll  = MatrixRotateZ(body.roll);
    body.transform = MatrixMultiply(MatrixMultiply(rotRoll, rotPitch), rotYaw);

    // Derive local axes from the matrix columns
    body.right   = {body.transform.m0, body.transform.m1, body.transform.m2};
    body.up      = {body.transform.m4, body.transform.m5, body.transform.m6};
    body.forward = Vector3Negate({body.transform.m8, body.transform.m9, body.transform.m10});

    // --- Thrust ---
    float thrustScale = params.thrustForce;
    if (input.boost) thrustScale *= params.boostMultiplier;

    Vector3 thrust = Vector3Scale(body.forward, input.thrust * thrustScale);
    Vector3 accel  = Vector3Scale(thrust, 1.0f / body.mass);
    body.velocity  = Vector3Add(body.velocity, Vector3Scale(accel, dt));

    // --- Drag ---
    float dragFactor = (input.thrust == 0.0f) ? params.brakeDrag : body.drag;
    // Continuous drag: v *= drag^dt
    float d = powf(dragFactor, dt);
    body.velocity = Vector3Scale(body.velocity, d);

    // --- Speed clamp ---
    float maxSpd = params.maxSpeed;
    if (input.boost) maxSpd *= params.boostMultiplier;
    float spd = body.speed();
    if (spd > maxSpd) {
        body.velocity = Vector3Scale(Vector3Normalize(body.velocity), maxSpd);
    }

    // --- Position ---
    body.position = Vector3Add(body.position, Vector3Scale(body.velocity, dt));
}

void FlightPhysics::IntegrateToward(RigidBody& body, Vector3 target,
                                    const FlightParams& params, float dt,
                                    float aggressionSpeed) {
    Vector3 toTarget = Vector3Subtract(target, body.position);
    float dist = Vector3Length(toTarget);
    if (dist < 0.01f) return;

    Vector3 desiredFwd = Vector3Normalize(toTarget);

    // Steer body.forward toward desiredFwd
    Vector3 cross = Vector3CrossProduct(body.forward, desiredFwd);
    float dot = Vector3DotProduct(body.forward, desiredFwd);
    float angle = acosf(Clamp(dot, -1.0f, 1.0f));

    if (Vector3Length(cross) > 0.001f) {
        Vector3 axis = Vector3Normalize(cross);
        float steerRate = params.yawRate * aggressionSpeed;
        float step = fminf(steerRate * dt, angle);
        Matrix rot = MatrixRotate(axis, step);

        Vector3 newFwd = Vector3Transform(body.forward, rot);
        // Recompute yaw/pitch from newFwd (simplified: just store forward and rebuild)
        // Approximate: decompose desired forward into yaw/pitch
        body.yaw   = atan2f(newFwd.x, -newFwd.z);
        body.pitch = asinf(Clamp(newFwd.y, -1.0f, 1.0f));
    }

    // Rebuild transform
    Matrix rotYaw   = MatrixRotateY(body.yaw);
    Matrix rotPitch = MatrixRotateX(body.pitch);
    Matrix rotRoll  = MatrixRotateZ(body.roll);
    body.transform = MatrixMultiply(MatrixMultiply(rotRoll, rotPitch), rotYaw);
    body.right   = {body.transform.m0, body.transform.m1, body.transform.m2};
    body.up      = {body.transform.m4, body.transform.m5, body.transform.m6};
    body.forward = Vector3Negate({body.transform.m8, body.transform.m9, body.transform.m10});

    // Thrust toward target
    float thrustInput = (dist > 5.0f) ? 1.0f : 0.0f;
    Vector3 accel = Vector3Scale(body.forward,
                                 thrustInput * params.thrustForce * aggressionSpeed / body.mass);
    body.velocity = Vector3Add(body.velocity, Vector3Scale(accel, dt));

    float d = powf(body.drag, dt);
    body.velocity = Vector3Scale(body.velocity, d);

    float spd = body.speed();
    float maxSpd = params.maxSpeed * aggressionSpeed;
    if (spd > maxSpd) {
        body.velocity = Vector3Scale(Vector3Normalize(body.velocity), maxSpd);
    }

    body.position = Vector3Add(body.position, Vector3Scale(body.velocity, dt));
}
