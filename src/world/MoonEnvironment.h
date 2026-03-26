#pragma once
#include <vector>
#include "raylib.h"
#include "raymath.h"

class AssetManager;

// Static geometry bounding sphere for tunnel collision.
struct EnvCollider {
    Vector3 center;
    float   radius;
};

class MoonEnvironment {
public:
    // Must match tunnel_section_a.glb inner half-width (W=10 in gen script)
    static constexpr float TUNNEL_HALF_WIDTH = 9.0f;  // 10 - 1 wall thickness

    // Tunnel Z bounds (entrance to end)
    static constexpr float TUNNEL_Z_ENTER = -15.0f;
    static constexpr float TUNNEL_Z_END   = -130.0f;

    void Init(AssetManager& assets);

    // Draw all environment models.
    void Draw(AssetManager& assets) const;

    // Returns colliders for the tunnel walls (used by CollisionSystem).
    const std::vector<EnvCollider>& GetTunnelColliders() const;

    // Entry/exit tunnel zone check.
    bool IsInsideTunnel(Vector3 pos) const;

private:
    std::vector<EnvCollider> m_tunnelColliders;

    bool m_surfaceLoaded = false;
    bool m_tunnelLoaded  = false;
};
