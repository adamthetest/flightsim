#pragma once
#include <memory>
#include <vector>
#include "entities/PlayerShip.h"
#include "entities/EnemyShip.h"
#include "entities/Projectile.h"
#include "entities/Powerup.h"
#include "entities/TradeStation.h"
#include "world/MoonEnvironment.h"

class AssetManager;
class Game;

struct ResourceBundle {
    float reactorFuel = 1.0f;   // 0..1
    int   credits     = 100;
    int   neutrinos   = 0;
};

class World {
public:
    PlayerShip      player;
    ResourceBundle  resources;
    MoonEnvironment moonEnv;

    std::vector<std::unique_ptr<EnemyShip>>   enemies;
    std::vector<std::unique_ptr<Projectile>>  projectiles;
    std::vector<std::unique_ptr<Powerup>>     powerups;
    std::vector<std::unique_ptr<TradeStation>> tradeStations;

    bool playerDockedAt  = false;
    bool inSystem2       = false;  // true after warp to Alpha Station
    bool warpUnlocked    = false;  // true once player has sold a neutrino

    void Init(AssetManager& assets);
    void Update(float dt, const InputState& input, Game& game);

    // Teleport player to System 2 and set up the receiver beacon.
    void WarpToSystem2();

    // Spawn helpers used by WeaponSystem
    void SpawnProjectile(Vector3 pos, Vector3 vel, float damage,
                         EntityId ownerId, bool fromPlayer);

    // Remove inactive entities
    void PruneInactive();

private:
    void SpawnEnemies();
    void SpawnPowerups();
    void SpawnTradeStation();
};
