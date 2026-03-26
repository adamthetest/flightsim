#pragma once
#include <memory>
#include "core/InputManager.h"
#include "core/AssetManager.h"
#include "core/SceneManager.h"
#include "world/World.h"
#include "physics/CollisionSystem.h"
#include "systems/ReactorSystem.h"
#include "systems/WeaponSystem.h"
#include "systems/TradingSystem.h"
#include "systems/PowerupSystem.h"
#include "rendering/Renderer.h"
#include "rendering/HUD.h"
#include "rendering/ParticleSystem.h"
#include "audio/AudioManager.h"

static constexpr int   SCREEN_W = 1280;
static constexpr int   SCREEN_H = 800;
static constexpr int   TARGET_FPS = 60;

enum class GameState { MENU, PLAYING, PAUSED, TRADE, WIN, DEAD };

class Game {
public:
    void Init();
    void Run();
    void Shutdown();

    GameState   GetState() const { return m_state; }
    void        SetState(GameState s) { m_state = s; }

    // Accessed by World::Update and systems
    AssetManager&  Assets()    { return m_assets; }
    AudioManager&  Audio()     { return m_audio; }
    ParticleSystem& Particles() { return m_particles; }
    SceneManager&  Scenes()    { return m_scenes; }

private:
    void Update(float dt);
    void Draw();

    void UpdatePlaying(float dt, const InputState& input);
    void ResolveCollisions();

    void DrawMenu() const;
    void DrawPauseOverlay() const;
    void DrawWinScreen() const;
    void DrawDeadScreen() const;

    GameState      m_state = GameState::MENU;
    int            m_pauseSel = 0;  // 0 = Resume, 1 = Quit

    InputManager   m_input;
    AssetManager   m_assets;
    World          m_world;
    Renderer       m_renderer;
    HUD            m_hud;
    ParticleSystem m_particles;
    AudioManager   m_audio;
    SceneManager   m_scenes;
};
