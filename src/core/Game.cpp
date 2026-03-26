#include "core/Game.h"
#include "rendering/ParticleSystem.h"
#include "systems/TradingSystem.h"
#include "raymath.h"
#include <cstdio>

void Game::Init() {
    InitWindow(SCREEN_W, SCREEN_H, "Moon Tunnels");
    SetTargetFPS(TARGET_FPS);
    SetExitKey(KEY_NULL); // handle exit ourselves

    m_audio.Init(m_assets);
    m_world.Init(m_assets);
    m_renderer.Init(m_assets);

    // Start ambient music (no-op if file missing)
    m_audio.SetMusic("assets/audio/music/space_ambient.ogg", m_assets);

    m_state = GameState::MENU;
}

void Game::Run() {
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f; // cap at 100ms to avoid physics explosion on focus loss

        m_audio.UpdateMusic();
        Update(dt);
        Draw();
    }
}

void Game::Shutdown() {
    m_renderer.Shutdown();
    m_assets.Unload();
    m_audio.Shutdown();
    CloseWindow();
}

void Game::Update(float dt) {
    InputState input = m_input.Poll();

    switch (m_state) {
        case GameState::MENU:
            if (input.menuConfirm) {
                m_state = GameState::PLAYING;
            }
            break;

        case GameState::PLAYING:
            if (input.pause) {
                m_state    = GameState::PAUSED;
                m_pauseSel = 0;
                break;
            }
            UpdatePlaying(dt, input);
            break;

        case GameState::PAUSED: {
            // Navigate with D-pad / left stick up-down
            bool up   = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)
                     || IsKeyPressed(KEY_UP)   || IsKeyPressed(KEY_W);
            bool down = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)
                     || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S);
            if (up)   m_pauseSel = 0;
            if (down) m_pauseSel = 1;

            // Confirm with A / Enter, or ESC always resumes
            if (input.pause) {
                m_state = GameState::PLAYING;
            } else if (input.menuConfirm || IsKeyPressed(KEY_ENTER)) {
                if (m_pauseSel == 0) {
                    m_state = GameState::PLAYING;
                } else {
                    Shutdown();
                    exit(0);
                }
            }
            break;
        }

        case GameState::TRADE: {
            // Simple trade input: keyboard 1/2/3 or face buttons
            if (IsKeyPressed(KEY_ONE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) {
                if (!m_world.tradeStations.empty()) {
                    TradingSystem::SellNeutrinos(m_world.resources,
                                                  *m_world.tradeStations[0], 1);
                }
            }
            if (IsKeyPressed(KEY_TWO) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)) {
                if (!m_world.tradeStations.empty()) {
                    TradingSystem::BuyShieldRepair(m_world.resources,
                                                    *m_world.tradeStations[0],
                                                    m_world.player.shields);
                }
            }
            if (IsKeyPressed(KEY_THREE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                if (!m_world.tradeStations.empty()) {
                    TradingSystem::BuyFuelReplenish(m_world.resources,
                                                     *m_world.tradeStations[0],
                                                     m_world.resources.reactorFuel);
                }
            }
            if ((IsKeyPressed(KEY_FOUR) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP))
                    && m_world.warpUnlocked && !m_world.inSystem2) {
                m_world.WarpToSystem2();
                m_state = GameState::PLAYING;
                m_world.playerDockedAt = false;
            }
            if (input.pause || input.menuBack) {
                m_state = GameState::PLAYING;
                m_world.playerDockedAt = false;
            }
            break;
        }

        case GameState::WIN:
        case GameState::DEAD:
            if (input.menuConfirm) {
                // Restart
                m_world = World();
                m_world.Init(m_assets);
                m_state = GameState::MENU;
            }
            break;
    }
}

void Game::UpdatePlaying(float dt, const InputState& input) {
    m_world.Update(dt, input, *this);

    // Particle thruster effect
    m_particles.EmitThruster(
        Vector3Subtract(m_world.player.position,
                        Vector3Scale(m_world.player.body.forward, 2.5f)),
        m_world.player.body.forward,
        input.thrust,
        {255, 150, 50, 200});

    m_particles.Update(dt);

    // Thruster SFX — replay each frame it isn't already playing so it loops
    if (fabsf(input.thrust) > 0.05f) {
        Sound& snd = m_assets.GetSound("assets/audio/sfx/thruster.ogg");
        if (snd.stream.buffer && !IsSoundPlaying(snd)) {
            SetSoundVolume(snd, fabsf(input.thrust) * 0.7f);
            PlaySound(snd);
        }
    }
}

void Game::Draw() {
    PlayerShip& player = m_world.player;

    // Build HUD data
    HUDData hudData;
    hudData.reactorFuel = m_world.resources.reactorFuel;
    hudData.shields     = player.shields;
    hudData.speed       = player.body.speed();
    hudData.credits     = m_world.resources.credits;
    hudData.neutrinos   = m_world.resources.neutrinos;
    hudData.playerPos   = player.position;
    hudData.playerFwd   = player.body.forward;
    hudData.tradeMenuOpen = (m_state == GameState::TRADE);
    hudData.warpUnlocked  = m_world.warpUnlocked;
    hudData.inSystem2     = m_world.inSystem2;

    for (const auto& e : m_world.enemies) {
        if (e->active) hudData.enemyPositions.push_back(e->position);
    }
    for (const auto& ts : m_world.tradeStations) {
        if (ts->IsPlayerInDockRange(player.position)) {
            hudData.dockAvailable = true;
            if (m_state == GameState::TRADE && !m_world.tradeStations.empty()) {
                auto offer = TradingSystem::GetOffer(*m_world.tradeStations[0]);
                hudData.tradeNeutrinoPrice = offer.neutrinoSellPrice;
                hudData.tradeShieldCost    = offer.shieldRepairCost;
            }
        }
    }

    if (m_state == GameState::MENU) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawMenu();
        EndDrawing();
        return;
    }

    m_renderer.BeginFrame(player.position,
                          player.body.forward,
                          player.body.up);
    m_renderer.Draw3D(m_world, m_assets);

    // End 3D before 2D overlay
    EndMode3D();

    // Particles: projected to screen space (much cheaper than DrawSphere in 3D)
    m_particles.Draw(m_renderer.GetCamera());

    m_hud.Draw(hudData);

    if (m_state == GameState::PAUSED)  DrawPauseOverlay();
    if (m_state == GameState::WIN)     DrawWinScreen();
    if (m_state == GameState::DEAD)    DrawDeadScreen();

    // FPS counter (debug)
    DrawFPS(SCREEN_W - 80, 8);

    EndDrawing();
}

void Game::DrawMenu() const {
    int cx = SCREEN_W / 2;
    DrawText("MOON TUNNELS", cx - 160, 200, 48, WHITE);
    DrawText("Space Combat | Neutrino Reactor | Solar Trade",
             cx - 220, 270, 18, GRAY);
    DrawText("Press ENTER / A  to Begin", cx - 130, 380, 20, GREEN);
    DrawText("W/S  Left stick up/down   : Look up / down", cx - 160, 480, 14, DARKGRAY);
    DrawText("A/D  Left stick left/right: Look left / right", cx - 160, 500, 14, DARKGRAY);
    DrawText("SHIFT / X button          : Thrust", cx - 160, 520, 14, DARKGRAY);
    DrawText("CTRL / L1                 : Boost", cx - 160, 540, 14, DARKGRAY);
    DrawText("SPACE / R1                : Fire", cx - 160, 560, 14, DARKGRAY);
    DrawText("F / A                     : Interact / Dock", cx - 160, 580, 14, DARKGRAY);
}

void Game::DrawPauseOverlay() const {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 140});
    DrawText("PAUSED", SCREEN_W / 2 - 60, SCREEN_H / 2 - 60, 40, WHITE);

    const char* items[]  = { "Resume", "Quit" };
    for (int i = 0; i < 2; i++) {
        bool selected = (m_pauseSel == i);
        Color c = selected ? YELLOW : GRAY;
        int y = SCREEN_H / 2 + i * 44;
        if (selected) DrawText(">", SCREEN_W / 2 - 80, y, 28, YELLOW);
        DrawText(items[i], SCREEN_W / 2 - 50, y, 28, c);
    }
    DrawText("D-pad / A to confirm   ESC to resume", SCREEN_W / 2 - 170, SCREEN_H / 2 + 110, 14, DARKGRAY);
}

void Game::DrawWinScreen() const {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 150});
    DrawText("MISSION COMPLETE", SCREEN_W / 2 - 160, SCREEN_H / 2 - 40, 40, GOLD);
    const char* sub = m_world.inSystem2
        ? "Neutrinos delivered to Alpha Station. The reactor lives."
        : "Trade complete.";
    int subW = MeasureText(sub, 18);
    DrawText(sub, SCREEN_W / 2 - subW / 2, SCREEN_H / 2 + 20, 18, WHITE);
    DrawText("Press ENTER to play again", SCREEN_W / 2 - 120, SCREEN_H / 2 + 60, 18, GREEN);
}

void Game::DrawDeadScreen() const {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {60, 0, 0, 160});
    DrawText("REACTOR FAILURE", SCREEN_W / 2 - 150, SCREEN_H / 2 - 40, 40, RED);
    DrawText("The ship is lost to the dark.", SCREEN_W / 2 - 130, SCREEN_H / 2 + 20, 20, WHITE);
    DrawText("Press ENTER to try again", SCREEN_W / 2 - 110, SCREEN_H / 2 + 60, 18, GRAY);
}
