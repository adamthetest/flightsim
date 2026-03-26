#pragma once
#include <string>
#include "raylib.h"

// Represents one solar system location reachable via warp.
struct SolarSystemData {
    std::string name;
    Vector3     spawnPos;        // player spawn when warping in
    std::string skyboxTexture;   // path to cubemap png
    bool        hasTradeStation;
};

class SolarSystem {
public:
    static SolarSystemData Home();
    static SolarSystemData AlphaStation(); // warp destination for v1 trade
};
