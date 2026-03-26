#pragma once

// Scene transitions sit on top of the Game state machine.
// For MVP, this is a thin wrapper; it exists to be expanded in v2.
enum class Scene {
    MAIN_MENU,
    MOON_SURFACE,   // open space + moon exterior
    TUNNEL,         // inside moon tunnel network
    TRADE_STATION,  // docked at trade station
    SOLAR_SYSTEM_2, // warp destination
    GAME_OVER,
    WIN,
};

class Game; // core/Game.h

class SceneManager {
public:
    void TransitionTo(Scene next, Game& game);
    Scene Current() const { return m_current; }

private:
    Scene m_current = Scene::MAIN_MENU;
};
