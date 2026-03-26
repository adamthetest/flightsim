#pragma once
#include <string>
#include "raylib.h"
#include "raymath.h"

class AssetManager;
class World;
class ParticleSystem;

class Renderer {
public:
    void Init(AssetManager& assets);
    void Shutdown();

    // Call once per frame before 3D drawing.
    void BeginFrame(const Vector3& shipPos, const Vector3& shipForward,
                    const Vector3& shipUp);

    // Draw all 3D world content between BeginFrame/EndFrame.
    void Draw3D(World& world, AssetManager& assets);

    void EndFrame();

    Camera3D GetCamera() const { return m_camera; }

private:
    Camera3D m_camera = {};

    // Skybox
    Model     m_skyboxModel = {};
    Shader    m_skyboxShader = {};
    bool      m_skyboxReady  = false;

    void DrawSkybox();
    void DrawEntity(AssetManager& assets, const std::string& modelKey,
                    Vector3 position, Matrix rotation, float scale = 1.0f);
    void LoadSkybox(AssetManager& assets);
};
