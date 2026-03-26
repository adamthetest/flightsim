#include "rendering/HUD.h"
#include "core/Game.h"  // SCREEN_W, SCREEN_H
#include <cmath>
#include <cstdio>

void HUD::DrawBar(int x, int y, int w, int h,
                  float fraction, Color fill, Color bg,
                  const char* label) const {
    DrawRectangle(x, y, w, h, bg);
    DrawRectangle(x, y, (int)(w * Clamp(fraction, 0.0f, 1.0f)), h, fill);
    DrawRectangleLines(x, y, w, h, WHITE);
    if (label) DrawText(label, x + 4, y + 2, h - 4, WHITE);
}

void HUD::DrawCrosshair() const {
    int cx = SCREEN_W / 2;
    int cy = SCREEN_H / 2;
    DrawLine(cx - 12, cy, cx - 4, cy, GREEN);
    DrawLine(cx + 4,  cy, cx + 12, cy, GREEN);
    DrawLine(cx, cy - 12, cx, cy - 4, GREEN);
    DrawLine(cx, cy + 4,  cx, cy + 12, GREEN);
    DrawCircleLines(cx, cy, 3, GREEN);
}

void HUD::DrawRadar(const HUDData& data, int cx, int cy, int radius) const {
    DrawCircleLines(cx, cy, (float)radius, {0, 255, 0, 80});
    DrawCircleLines(cx, cy, (float)(radius / 2), {0, 255, 0, 40});

    // Player dot
    DrawCircle(cx, cy, 3, GREEN);

    // Enemy dots — project relative positions onto 2D radar plane
    // Use player forward as +Y on radar, right as +X
    Vector3 fwd   = Vector3Normalize({data.playerFwd.x, 0, data.playerFwd.z});
    if (Vector3Length(fwd) < 0.01f) fwd = {0, 0, -1};
    Vector3 right = Vector3Normalize(Vector3CrossProduct({0, 1, 0}, fwd));
    right = Vector3Negate(right); // flip so right is right

    constexpr float RADAR_RANGE = 300.0f;

    // Enemy dots — sized and clamped to radar range
    for (const auto& epos : data.enemyPositions) {
        Vector3 rel = Vector3Subtract(epos, data.playerPos);
        float dx = Vector3DotProduct(rel, right);
        float dz = Vector3DotProduct(rel, fwd);
        float dist = sqrtf(dx * dx + dz * dz);
        if (dist > RADAR_RANGE) {
            dx = dx / dist * RADAR_RANGE;
            dz = dz / dist * RADAR_RANGE;
        }
        int sx = cx + (int)(dx / RADAR_RANGE * radius);
        int sy = cy - (int)(dz / RADAR_RANGE * radius);
        DrawCircle(sx, sy, 3, RED);
    }

    // Planet / star blips — always pinned to the radar edge (direction indicator only)
    for (const auto& blip : data.planetBlips) {
        Vector3 rel = Vector3Subtract(blip.pos, data.playerPos);
        float dx = Vector3DotProduct(rel, right);
        float dz = Vector3DotProduct(rel, fwd);
        float dist = sqrtf(dx * dx + dz * dz);
        if (dist < 0.1f) continue;
        // Pin to 92% of radar radius so they sit just inside the outer ring
        int sx = cx + (int)(dx / dist * radius * 0.92f);
        int sy = cy - (int)(dz / dist * radius * 0.92f);
        DrawCircle(sx, sy, 3, blip.color);
        DrawCircleLines((float)sx, (float)sy, 4.5f, blip.color);
    }
}

void HUD::DrawTradeMenu(const HUDData& data) const {
    int panelH = data.warpUnlocked ? 280 : 240;
    int panelX = SCREEN_W / 2 - 200;
    int panelY = SCREEN_H / 2 - panelH / 2;
    DrawRectangle(panelX, panelY, 400, panelH, {0, 0, 0, 200});
    DrawRectangleLines(panelX, panelY, 400, panelH, GOLD);
    DrawText("TRADE STATION", panelX + 100, panelY + 10, 20, GOLD);

    char buf[128];
    snprintf(buf, sizeof(buf), "Credits: %d   Neutrinos: %d",
             data.credits, data.neutrinos);
    DrawText(buf, panelX + 10, panelY + 45, 16, WHITE);

    snprintf(buf, sizeof(buf), "[1] Sell neutrino  +%d cr", data.tradeNeutrinoPrice);
    DrawText(buf, panelX + 10, panelY + 80, 16, SKYBLUE);

    snprintf(buf, sizeof(buf), "[2] Repair shields  -%d cr", data.tradeShieldCost);
    DrawText(buf, panelX + 10, panelY + 110, 16, SKYBLUE);

    snprintf(buf, sizeof(buf), "[3] Refuel reactor  -30 cr");
    DrawText(buf, panelX + 10, panelY + 140, 16, SKYBLUE);

    if (data.warpUnlocked) {
        DrawText("[4] WARP to Alpha Station  (Y button)", panelX + 10, panelY + 180, 16, GOLD);
        DrawText("Deliver your neutrinos to win!", panelX + 10, panelY + 202, 13, GRAY);
    }

    DrawText("[ESC] Leave", panelX + 10, panelY + panelH - 30, 14, GRAY);
}

void HUD::Draw(const HUDData& data) const {
    // Reactor bar
    Color reactorColor = (data.reactorFuel > 0.3f) ? GREEN
                       : (data.reactorFuel > 0.15f) ? YELLOW : RED;
    DrawBar(20, 20, 200, 20, data.reactorFuel, reactorColor, {30, 30, 30, 200}, "REACTOR");

    // Shield bar
    DrawBar(20, 50, 200, 20, data.shields, SKYBLUE, {30, 30, 30, 200}, "SHIELD");

    // Speed
    char speedBuf[32];
    snprintf(speedBuf, sizeof(speedBuf), "SPD %.0f", data.speed);
    DrawText(speedBuf, 20, 80, 16, WHITE);

    // Credits + neutrinos
    char resBuf[48];
    snprintf(resBuf, sizeof(resBuf), "CR %d  NU %d", data.credits, data.neutrinos);
    DrawText(resBuf, 20, 100, 16, GOLD);

    // System name (top-centre)
    const char* sysName = data.inSystem2 ? "ALPHA STATION SYSTEM" : "SOL / MOON";
    int sysW = MeasureText(sysName, 14);
    DrawText(sysName, SCREEN_W / 2 - sysW / 2, 8, 14, data.inSystem2 ? GOLD : GRAY);

    // Crosshair
    DrawCrosshair();

    // Radar (bottom-right)
    int radarCX = SCREEN_W - 90;
    int radarCY = SCREEN_H - 90;
    DrawRadar(data, radarCX, radarCY, 70);

    // Dock hint
    if (data.dockAvailable) {
        DrawText("[F] Dock", SCREEN_W / 2 - 40, SCREEN_H / 2 + 60, 18, GOLD);
    }

    // Trade menu overlay
    if (data.tradeMenuOpen) {
        DrawTradeMenu(data);
    }
}
