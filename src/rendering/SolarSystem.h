#pragma once
#include "raylib.h"

// All solar system body data in one place so Renderer, HUD, and physics all agree.
struct PlanetDesc {
    const char* key;        // asset path for 3D model
    Vector3     pos;        // world-space position
    float       radius;     // must match GLB sphere radius
    Color       atmo;       // atmosphere halo colour (3D rendering)
    Color       radarColor; // blip colour on 2D radar
    float       gm;         // gravitational parameter (units³/s²); a = gm/r²
};

// Gravity tuning:
//   At r units away, acceleration = gm / r²  (units/s²)
//   Star  at r=1000 →  0.60 u/s²   at r=200 → 15 u/s²
//   Earth at r=200  →  0.75 u/s²   at r=80  →  4.7 u/s²
//   Gas   at r=300  →  0.56 u/s²   at r=80  →  7.8 u/s²
//   Ice   at r=200  →  0.35 u/s²
//   Rock  at r=100  →  0.10 u/s²
static const PlanetDesc PLANETS[] = {
    { "assets/models/environment/planet_rock.glb",
      { 140.0f, -25.0f,  820.0f },  8.0f,
      { 210, 150,  80, 255 }, { 180, 120,  60, 255 },  1000.0f },

    { "assets/models/environment/planet_earth.glb",
      {-380.0f,  85.0f,  660.0f }, 15.0f,
      {  80, 155, 255, 255 }, {  80, 140, 255, 255 }, 30000.0f },

    { "assets/models/environment/planet_gas.glb",
      { 520.0f, -55.0f, 1380.0f }, 45.0f,
      { 235, 175,  85, 255 }, { 210, 150,  60, 255 }, 50000.0f },

    { "assets/models/environment/planet_ice.glb",
      {-720.0f,  35.0f, 1520.0f }, 22.0f,
      { 145, 195, 255, 255 }, { 120, 170, 240, 255 }, 14000.0f },
};

static const Vector3   STAR_POS    = {0.0f, 0.0f, 1000.0f};
static constexpr float STAR_RADIUS = 40.0f;
static constexpr float STAR_GM     = 600000.0f;
static constexpr int   PLANET_COUNT = 4;
