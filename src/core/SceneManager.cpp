#include "core/SceneManager.h"
#include "core/Game.h"

void SceneManager::TransitionTo(Scene next, Game& game) {
    m_current = next;
    switch (next) {
        case Scene::MAIN_MENU:
            game.SetState(GameState::MENU);
            break;
        case Scene::GAME_OVER:
            game.SetState(GameState::DEAD);
            break;
        case Scene::WIN:
            game.SetState(GameState::WIN);
            break;
        default:
            game.SetState(GameState::PLAYING);
            break;
    }
}
