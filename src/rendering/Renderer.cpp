#include "rendering/Renderer.h"
#include "rendering/SolarSystem.h"
#include "core/AssetManager.h"
#include "world/World.h"
#include "rendering/ParticleSystem.h"
#include "raymath.h"
#include "rlgl.h"
#include <string>

void Renderer::Init(AssetManager& assets) {
    m_camera.fovy       = 70.0f;
    m_camera.projection = CAMERA_PERSPECTIVE;
    m_camera.up         = {0.0f, 1.0f, 0.0f};
    m_camera.position   = {0.0f, 5.0f, 20.0f};
    m_camera.target     = {0.0f, 0.0f,  0.0f};

    LoadSkybox(assets);
}

void Renderer::LoadSkybox(AssetManager& assets) {
    const char* cubemapPath = "assets/textures/skybox/space_cubemap.png";
    if (!FileExists(cubemapPath)) {
        m_skyboxReady = false;
        return;
    }

    m_skyboxShader = assets.GetShader("skybox",
        "assets/shaders/skybox.vs",
        "assets/shaders/skybox.fs");

    int envMap = MATERIAL_MAP_CUBEMAP;
    SetShaderValue(m_skyboxShader,
        GetShaderLocation(m_skyboxShader, "environmentMap"),
        &envMap, SHADER_UNIFORM_INT);

    Mesh cube = GenMeshCube(1.0f, 1.0f, 1.0f);
    m_skyboxModel = LoadModelFromMesh(cube);
    m_skyboxModel.materials[0].shader = m_skyboxShader;

    Image img = LoadImage(cubemapPath);
    m_skyboxModel.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture =
        LoadTextureCubemap(img, CUBEMAP_LAYOUT_AUTO_DETECT);
    UnloadImage(img);

    m_skyboxReady = true;
}

void Renderer::DrawSkybox() {
    if (!m_skyboxReady) {
        ClearBackground({5, 5, 15, 255}); // deep space black
        return;
    }
    rlDisableBackfaceCulling();
    rlDisableDepthMask();
    DrawModel(m_skyboxModel, m_camera.position, 1.0f, WHITE);
    rlEnableBackfaceCulling();
    rlEnableDepthMask();
}

void Renderer::DrawEntity(AssetManager& assets, const std::string& modelKey,
                          Vector3 position, Matrix rotation, float scale) {
    if (modelKey.empty()) return;
    Model& model = assets.GetModel(modelKey);

    // Temporarily set model.transform to our rotation; restore after.
    Matrix saved = model.transform;
    model.transform = rotation;
    DrawModel(model, position, scale, WHITE);
    model.transform = saved;
}

void Renderer::BeginFrame(const Vector3& shipPos, const Vector3& shipForward,
                          const Vector3& shipUp) {
    // Chase camera: offset behind and slightly above ship
    const float followDist  = 14.0f;
    const float heightOffset = 3.5f;

    Vector3 camPos = Vector3Subtract(shipPos,
                         Vector3Scale(shipForward, followDist));
    camPos = Vector3Add(camPos, Vector3Scale(shipUp, heightOffset));

    m_camera.position = camPos;
    m_camera.target   = Vector3Add(shipPos, Vector3Scale(shipForward, 4.0f));
    m_camera.up       = shipUp;

    BeginDrawing();
    ClearBackground({5, 5, 15, 255});
    BeginMode3D(m_camera);
    DrawSkybox();
}

void Renderer::Draw3D(World& world, AssetManager& assets) {
    // Star — draw corona rings first (largest to smallest) so the bright core
    // model renders on top. Position defined in SolarSystem.h.
    DrawSphereEx(STAR_POS, 70.0f, 8, 12,  {255,  90,   5, 255}); // outer corona
    DrawSphereEx(STAR_POS, 56.0f, 10, 14, {255, 160,  30, 255}); // mid glow
    DrawSphereEx(STAR_POS, 44.0f, 12, 16, {255, 220,  80, 255}); // inner halo
    DrawEntity(assets, "assets/models/environment/star.glb",
               STAR_POS, MatrixIdentity());

    // Planets — atmosphere halo first (slightly larger), then model on top
    for (const PlanetDesc& p : PLANETS) {
        DrawSphereEx(p.pos, p.radius * 1.18f, 10, 14, p.atmo);
        DrawEntity(assets, p.key, p.pos, MatrixIdentity());
    }

    // Environment (surface + tunnel) — draw first (opaque background geometry)
    world.moonEnv.Draw(assets);

    // Player ship
    DrawEntity(assets, world.player.modelKey,
               world.player.position, world.player.rotation);

    // Enemies
    for (const auto& e : world.enemies) {
        if (e->active) {
            DrawEntity(assets, e->modelKey, e->position, e->rotation);
        }
    }

    // Projectiles — draw as small bright capsules / spheres
    for (const auto& p : world.projectiles) {
        if (!p->active) continue;
        Color c = p->fromPlayer ? YELLOW : RED;
        DrawSphere(p->position, 0.3f, c);
    }

    // Powerups
    for (const auto& pu : world.powerups) {
        if (!pu->active) continue;
        if (!pu->modelKey.empty()) {
            DrawEntity(assets, pu->modelKey, pu->position, pu->rotation);
        } else {
            DrawSphere(pu->position, 1.2f, SKYBLUE);
        }
    }

    // Trade stations
    for (const auto& ts : world.tradeStations) {
        if (!ts->active) continue;
        DrawEntity(assets, ts->modelKey, ts->position, ts->rotation);
        // Dock range indicator (subtle ring on the ground plane)
        DrawCircle3D(ts->position, ts->dockRadius,
                     {1, 0, 0}, 90.0f, {0, 255, 255, 60});
    }
}

void Renderer::EndFrame() {
    EndMode3D();
    // HUD and particles are drawn by Game after this call (still inside BeginDrawing)
    EndDrawing();
}

void Renderer::Shutdown() {
    if (m_skyboxReady) {
        UnloadTexture(m_skyboxModel.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture);
        UnloadModel(m_skyboxModel);
    }
}
