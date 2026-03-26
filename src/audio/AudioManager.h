#pragma once
#include <string>
#include "raylib.h"

class AssetManager;

class AudioManager {
public:
    void Init(AssetManager& assets);
    void Shutdown();

    // Spatial SFX: volume scales with 1/distance. Max audible range = maxDist.
    void PlaySpatial(const std::string& soundKey, AssetManager& assets,
                     float distance, float maxDist = 200.0f);

    // Non-spatial SFX (UI, pickup, etc.)
    void PlayUI(const std::string& soundKey, AssetManager& assets);

    // Background music streaming (loops).
    void SetMusic(const std::string& musicKey, AssetManager& assets);
    void UpdateMusic(); // call every frame

    void SetMasterVolume(float v);
    void SetMusicVolume(float v);
    void SetSFXVolume(float v);

private:
    Music* m_currentMusic = nullptr;
    float  m_musicVolume  = 0.6f;
    float  m_sfxVolume    = 0.8f;
    bool   m_initialized  = false;
};
