#include "world/MoonEnvironment.h"
#include "core/AssetManager.h"
#include <cmath>

void MoonEnvironment::Init(AssetManager& assets) {
    assets.GetModel("assets/models/environment/moon_surface.glb");
    assets.GetModel("assets/models/environment/tunnel_section_a.glb");

    m_surfaceLoaded = FileExists("assets/models/environment/moon_surface.glb");
    m_tunnelLoaded  = FileExists("assets/models/environment/tunnel_section_a.glb");

    // Sphere colliders are kept for non-player entities (enemies) that don't
    // use the AABB tunnel clamping. Spaced every 8 units along the tunnel.
    m_tunnelColliders.clear();
    const float tunnelLen = fabsf(TUNNEL_Z_END - TUNNEL_Z_ENTER);
    const int   steps     = (int)(tunnelLen / 8.0f);
    for (int i = 0; i <= steps; ++i) {
        float z = TUNNEL_Z_ENTER - (float)i * 8.0f;
        float hw = TUNNEL_HALF_WIDTH;
        m_tunnelColliders.push_back({{ 0.0f,  hw, z}, 2.0f}); // top
        m_tunnelColliders.push_back({{ 0.0f, -hw, z}, 2.0f}); // bottom
        m_tunnelColliders.push_back({{-hw,  0.0f, z}, 2.0f}); // left
        m_tunnelColliders.push_back({{ hw,  0.0f, z}, 2.0f}); // right
    }
}

void MoonEnvironment::Draw(AssetManager& assets) const {
    // Moon surface — always draw (fallback plane if model missing)
    if (m_surfaceLoaded) {
        Model& surf = assets.GetModel("assets/models/environment/moon_surface.glb");
        Matrix saved = surf.transform;
        surf.transform = MatrixIdentity();
        DrawModel(surf, {0.0f, -5.0f, 0.0f}, 1.0f, WHITE);
        surf.transform = saved;
    } else {
        DrawPlane({0.0f, -5.0f, 0.0f}, {600.0f, 600.0f}, {170, 170, 180, 255});
    }

    // Tunnel — center is halfway between ENTER and END
    float tunnelCenterZ = (TUNNEL_Z_ENTER + TUNNEL_Z_END) * 0.5f;
    if (m_tunnelLoaded) {
        Model& tun = assets.GetModel("assets/models/environment/tunnel_section_a.glb");
        Matrix saved = tun.transform;
        tun.transform = MatrixIdentity();
        DrawModel(tun, {0.0f, 0.0f, tunnelCenterZ}, 1.0f, WHITE);
        tun.transform = saved;
    } else {
        // Wireframe outline so it's visible without a model
        float hw = TUNNEL_HALF_WIDTH;
        float zE = TUNNEL_Z_ENTER;
        float zN = TUNNEL_Z_END;
        Color c = {100, 100, 130, 255};
        // 4 corner lines along Z
        DrawLine3D({-hw, -hw, zE}, {-hw, -hw, zN}, c);
        DrawLine3D({ hw, -hw, zE}, { hw, -hw, zN}, c);
        DrawLine3D({-hw,  hw, zE}, {-hw,  hw, zN}, c);
        DrawLine3D({ hw,  hw, zE}, { hw,  hw, zN}, c);
        // entrance rect
        DrawLine3D({-hw, -hw, zE}, { hw, -hw, zE}, c);
        DrawLine3D({ hw, -hw, zE}, { hw,  hw, zE}, c);
        DrawLine3D({ hw,  hw, zE}, {-hw,  hw, zE}, c);
        DrawLine3D({-hw,  hw, zE}, {-hw, -hw, zE}, c);
    }

    // Entrance marker — bright ring above the tunnel mouth
    DrawCircle3D({0.0f, 0.0f, TUNNEL_Z_ENTER},
                 TUNNEL_HALF_WIDTH * 1.2f, {1, 0, 0}, 90.0f,
                 {0, 220, 255, 120});
}

const std::vector<EnvCollider>& MoonEnvironment::GetTunnelColliders() const {
    return m_tunnelColliders;
}

bool MoonEnvironment::IsInsideTunnel(Vector3 pos) const {
    return (pos.z < TUNNEL_Z_ENTER &&
            pos.z > TUNNEL_Z_END   &&
            fabsf(pos.x) < TUNNEL_HALF_WIDTH + 2.0f &&
            fabsf(pos.y) < TUNNEL_HALF_WIDTH + 2.0f);
}
