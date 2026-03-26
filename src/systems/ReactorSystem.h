#pragma once

struct ResourceBundle;
class PlayerShip;

class ReactorSystem {
public:
    static constexpr float BASE_DRAIN_RATE    = 0.008f; // per second at idle
    static constexpr float WEAPON_DRAIN_BONUS = 0.004f; // extra per second when firing
    static constexpr float SHIELD_DRAIN_BONUS = 0.003f; // extra per second with shields up

    // Drains reactor fuel based on activity. Returns true if reactor has died.
    static bool Update(ResourceBundle& resources, const PlayerShip& player,
                       bool weaponActive, float dt);
};
