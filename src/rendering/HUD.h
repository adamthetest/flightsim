#pragma once
#include <vector>
#include "raylib.h"
#include "raymath.h"

struct RadarBlip { Vector3 pos; Color color; };

// Plain data the HUD needs — avoids coupling to World.
struct HUDData {
    float    reactorFuel  = 1.0f;   // 0..1
    float    shields      = 1.0f;   // 0..1
    float    speed        = 0.0f;   // world units/sec
    int      credits      = 0;
    int      neutrinos    = 0;
    Vector3  playerPos    = {0,0,0};
    Vector3  playerFwd    = {0,0,-1};
    std::vector<Vector3>    enemyPositions;
    std::vector<RadarBlip>  planetBlips;   // solar system bodies shown on radar
    bool     dockAvailable     = false;
    bool     tradeMenuOpen     = false;
    bool     warpUnlocked      = false;
    bool     inSystem2         = false;
    int      tradeNeutrinoPrice = 5;
    int      tradeShieldCost    = 20;
    int      tradeFuelCost      = 30;
};

class HUD {
public:
    void Draw(const HUDData& data) const;

private:
    void DrawBar(int x, int y, int w, int h,
                 float fraction, Color fill, Color bg, const char* label) const;
    void DrawRadar(const HUDData& data, int cx, int cy, int radius) const;
    void DrawTradeMenu(const HUDData& data) const;
    void DrawCrosshair() const;
};
