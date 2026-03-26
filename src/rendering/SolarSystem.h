#pragma once
#include "raylib.h"

// All solar system body data in one place so Renderer and HUD both see the same positions.
struct PlanetDesc {
    const char* key;        // asset path for 3D model
    Vector3     pos;        // world-space position
    float       radius;     // must match GLB sphere radius
    Color       atmo;       // atmosphere halo colour (3D rendering)
    Color       radarColor; // blip colour on 2D radar
};

static const PlanetDesc PLANETS[] = {
    { "assets/models/environment/planet_rock.glb",
      { 140.0f, -25.0f,  820.0f },  8.0f,
      { 210, 150,  80, 255 }, { 180, 120,  60, 255 } },

    { "assets/models/environment/planet_earth.glb",
      {-380.0f,  85.0f,  660.0f }, 15.0f,
      {  80, 155, 255, 255 }, {  80, 140, 255, 255 } },

    { "assets/models/environment/planet_gas.glb",
      { 520.0f, -55.0f, 1380.0f }, 45.0f,
      { 235, 175,  85, 255 }, { 210, 150,  60, 255 } },

    { "assets/models/environment/planet_ice.glb",
      {-720.0f,  35.0f, 1520.0f }, 22.0f,
      { 145, 195, 255, 255 }, { 120, 170, 240, 255 } },
};

static const Vector3 STAR_POS    = {0.0f, 0.0f, 1000.0f};
static constexpr float STAR_RADIUS = 40.0f;
static constexpr int   PLANET_COUNT = 4;
