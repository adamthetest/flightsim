#pragma once

// Normalized input state produced each frame.
// Values in range [-1, 1] for axes, bool for buttons.
struct InputState {
    float thrust = 0.0f;    // +1 = full forward, -1 = full reverse
    float pitch  = 0.0f;    // +1 = nose up
    float yaw    = 0.0f;    // +1 = nose right
    float roll   = 0.0f;    // +1 = roll right (clockwise from behind)
    bool  fire     = false;
    bool  boost    = false;
    bool  interact = false; // dock/trade
    bool  pause    = false;
    bool  menuConfirm = false;
    bool  menuBack    = false;
};

class InputManager {
public:
    // Poll gamepad (controller 0) with keyboard fallback.
    InputState Poll();

private:
    float Deadzone(float value, float threshold = 0.15f) const;
};
