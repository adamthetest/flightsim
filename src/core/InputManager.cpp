#include "core/InputManager.h"
#include "raylib.h"
#include "raymath.h"
#include <cmath>

float InputManager::Deadzone(float value, float threshold) const {
    if (fabsf(value) < threshold) return 0.0f;
    float sign = (value > 0.0f) ? 1.0f : -1.0f;
    return sign * (fabsf(value) - threshold) / (1.0f - threshold);
}

InputState InputManager::Poll() {
    InputState s;

    const int GP = 0;
    const bool gpConnected = IsGamepadAvailable(GP);

    if (gpConnected) {
        // Left stick: look direction (yaw + pitch)
        float lx = Deadzone(GetGamepadAxisMovement(GP, GAMEPAD_AXIS_LEFT_X));
        float ly = Deadzone(GetGamepadAxisMovement(GP, GAMEPAD_AXIS_LEFT_Y));
        s.yaw   = -lx;
        s.pitch = -ly;  // stick forward (−Y) = look up (+pitch)

        // X button (left face) = thrust, R1 = fire, A = interact, Start = pause
        s.thrust   = IsGamepadButtonDown(GP, GAMEPAD_BUTTON_RIGHT_FACE_LEFT) ? 1.0f : 0.0f; // X
        s.fire     = IsGamepadButtonDown(GP, GAMEPAD_BUTTON_RIGHT_TRIGGER_1);   // R1
        s.boost    = IsGamepadButtonDown(GP, GAMEPAD_BUTTON_LEFT_TRIGGER_1);    // L1
        s.interact = IsGamepadButtonPressed(GP, GAMEPAD_BUTTON_RIGHT_FACE_DOWN); // A
        s.pause    = IsGamepadButtonPressed(GP, GAMEPAD_BUTTON_MIDDLE_RIGHT);    // Start
        s.menuConfirm = IsGamepadButtonPressed(GP, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
        s.menuBack    = IsGamepadButtonPressed(GP, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);
    }

    // Keyboard
    //   W / S        look up / down   (pitch)
    //   A / D        look left / right (yaw)
    //   Q / E        roll
    //   Left Shift   thrust
    //   Space        fire
    //   F            interact / dock
    if (IsKeyDown(KEY_W))           s.pitch  += 1.0f;
    if (IsKeyDown(KEY_S))           s.pitch  -= 1.0f;
    if (IsKeyDown(KEY_A))           s.yaw    -= 1.0f;
    if (IsKeyDown(KEY_D))           s.yaw    += 1.0f;
    if (IsKeyDown(KEY_LEFT_SHIFT))  s.thrust  = 1.0f;
    if (IsKeyDown(KEY_SPACE))       s.fire    = true;
    if (IsKeyDown(KEY_LEFT_CONTROL)) s.boost  = true;
    if (IsKeyPressed(KEY_F))        s.interact = true;
    if (IsKeyPressed(KEY_ESCAPE))   s.pause   = true;
    if (IsKeyPressed(KEY_ENTER))    s.menuConfirm = true;

    s.thrust = Clamp(s.thrust, -1.0f, 1.0f);
    s.pitch  = Clamp(s.pitch,  -1.0f, 1.0f);
    s.yaw    = Clamp(s.yaw,    -1.0f, 1.0f);
    s.roll   = Clamp(s.roll,   -1.0f, 1.0f);

    return s;
}
