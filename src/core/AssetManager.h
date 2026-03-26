#pragma once
#include <string>
#include <unordered_map>
#include "raylib.h"

// Loads and caches Raylib assets keyed by string.
// Call Unload() before CloseWindow().
class AssetManager {
public:
    ~AssetManager();

    // Returns a reference to the cached model (loaded on first call).
    // If path does not exist, returns a fallback box model.
    Model&   GetModel(const std::string& key, const std::string& path);

    // Convenience: key == path.
    Model&   GetModel(const std::string& path);

    Texture2D& GetTexture(const std::string& key, const std::string& path);
    Texture2D& GetTexture(const std::string& path);

    Sound&   GetSound(const std::string& key, const std::string& path);
    Sound&   GetSound(const std::string& path);

    Music&   GetMusic(const std::string& key, const std::string& path);
    Music&   GetMusic(const std::string& path);

    Shader&  GetShader(const std::string& key,
                       const std::string& vsPath,
                       const std::string& fsPath);

    void Unload();

private:
    std::unordered_map<std::string, Model>    m_models;
    std::unordered_map<std::string, Texture2D> m_textures;
    std::unordered_map<std::string, Sound>    m_sounds;
    std::unordered_map<std::string, Music>    m_music;
    std::unordered_map<std::string, Shader>   m_shaders;
};
