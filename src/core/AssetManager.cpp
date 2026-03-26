#include "core/AssetManager.h"
#include <cstdio>

AssetManager::~AssetManager() {
    Unload();
}

Model& AssetManager::GetModel(const std::string& key, const std::string& path) {
    auto it = m_models.find(key);
    if (it != m_models.end()) return it->second;

    Model m;
    if (FileExists(path.c_str())) {
        m = LoadModel(path.c_str());
    } else {
        // Fallback: generate a box so Phase 1 works without real assets
        m = LoadModelFromMesh(GenMeshCube(2.0f, 1.0f, 4.0f));
    }
    m_models.emplace(key, m);
    return m_models.at(key);
}

Model& AssetManager::GetModel(const std::string& path) {
    return GetModel(path, path);
}

Texture2D& AssetManager::GetTexture(const std::string& key, const std::string& path) {
    auto it = m_textures.find(key);
    if (it != m_textures.end()) return it->second;

    Texture2D t;
    if (FileExists(path.c_str())) {
        t = LoadTexture(path.c_str());
    } else {
        // 1x1 white placeholder
        Image img = GenImageColor(1, 1, WHITE);
        t = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    m_textures.emplace(key, t);
    return m_textures.at(key);
}

Texture2D& AssetManager::GetTexture(const std::string& path) {
    return GetTexture(path, path);
}

Sound& AssetManager::GetSound(const std::string& key, const std::string& path) {
    auto it = m_sounds.find(key);
    if (it != m_sounds.end()) return it->second;

    Sound snd = {};
    if (FileExists(path.c_str())) {
        snd = LoadSound(path.c_str());
    }
    m_sounds.emplace(key, snd);
    return m_sounds.at(key);
}

Sound& AssetManager::GetSound(const std::string& path) {
    return GetSound(path, path);
}

Music& AssetManager::GetMusic(const std::string& key, const std::string& path) {
    auto it = m_music.find(key);
    if (it != m_music.end()) return it->second;

    Music mus = {};
    if (FileExists(path.c_str())) {
        mus = LoadMusicStream(path.c_str());
    }
    m_music.emplace(key, mus);
    return m_music.at(key);
}

Music& AssetManager::GetMusic(const std::string& path) {
    return GetMusic(path, path);
}

Shader& AssetManager::GetShader(const std::string& key,
                                 const std::string& vsPath,
                                 const std::string& fsPath) {
    auto it = m_shaders.find(key);
    if (it != m_shaders.end()) return it->second;

    const char* vs = FileExists(vsPath.c_str()) ? vsPath.c_str() : nullptr;
    const char* fs = FileExists(fsPath.c_str()) ? fsPath.c_str() : nullptr;
    Shader sh = LoadShader(vs, fs);
    m_shaders.emplace(key, sh);
    return m_shaders.at(key);
}

void AssetManager::Unload() {
    for (auto& [k, m] : m_models)   UnloadModel(m);
    for (auto& [k, t] : m_textures) UnloadTexture(t);
    for (auto& [k, s] : m_sounds)   UnloadSound(s);
    for (auto& [k, m] : m_music)    UnloadMusicStream(m);
    for (auto& [k, s] : m_shaders)  UnloadShader(s);
    m_models.clear();
    m_textures.clear();
    m_sounds.clear();
    m_music.clear();
    m_shaders.clear();
}
