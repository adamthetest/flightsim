#include "world/SolarSystem.h"

SolarSystemData SolarSystem::Home() {
    return {
        "Sol / Moon",
        {0.0f, 5.0f, 0.0f},
        "assets/textures/skybox/space_cubemap.png",
        false
    };
}

SolarSystemData SolarSystem::AlphaStation() {
    return {
        "Alpha Station",
        {500.0f, 0.0f, 0.0f},
        "assets/textures/skybox/space_cubemap.png",
        true
    };
}
