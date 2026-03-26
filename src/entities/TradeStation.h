#pragma once
#include "Entity.h"

class TradeStation : public Entity {
public:
    TradeStation();

    float dockRadius  = 15.0f;  // distance within which player can dock
    int   buyNeutrinos  = 5;    // credits per neutrino sold
    int   sellShields   = 20;   // credits to buy shield repair
    float spinSpeed   = 0.3f;   // rotation speed for visual effect
    float spinAngle   = 0.0f;

    void Update(float dt);

    bool IsPlayerInDockRange(Vector3 playerPos) const;
};
