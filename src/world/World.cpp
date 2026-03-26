#include "world/World.h"
#include "core/Game.h"
#include "core/InputManager.h"
#include "core/AssetManager.h"
#include "physics/FlightPhysics.h"
#include "physics/CollisionSystem.h"
#include "systems/ReactorSystem.h"
#include "systems/WeaponSystem.h"
#include "systems/PowerupSystem.h"
#include "ai/EnemyAI.h"
#include "raymath.h"
#include <algorithm>

void World::SpawnProjectile(Vector3 pos, Vector3 vel, float damage,
                            EntityId ownerId, bool fromPlayer) {
    auto proj = std::make_unique<Projectile>();
    proj->position   = pos;
    proj->velocity   = vel;
    proj->damage     = damage;
    proj->ownerId    = ownerId;
    proj->fromPlayer = fromPlayer;
    proj->modelKey   = "";
    projectiles.push_back(std::move(proj));
}

void World::SpawnEnemies() {
    // Spawn 3 pirate ships at different positions around the moon
    const Vector3 spawnPositions[] = {
        {80.0f,  0.0f,  50.0f},
        {-60.0f, 20.0f, 80.0f},
        {30.0f, -10.0f, 100.0f},
    };
    for (const auto& pos : spawnPositions) {
        auto e = std::make_unique<EnemyShip>();
        e->body.position = pos;
        e->ai.patrolTarget = pos;
        enemies.push_back(std::move(e));
    }
}

void World::SpawnPowerups() {
    // Neutrino canisters inside the tunnel
    const Vector3 pickupPositions[] = {
        {0.0f, 0.0f, -40.0f},
        {2.0f, 0.0f, -70.0f},
        {-1.0f, 0.0f, -100.0f},
    };
    for (const auto& pos : pickupPositions) {
        auto pu = std::make_unique<Powerup>(PowerupType::NEUTRINO_CANISTER);
        pu->position = pos;
        pu->baseY    = pos.y;
        powerups.push_back(std::move(pu));
    }
}

void World::SpawnTradeStation() {
    auto ts = std::make_unique<TradeStation>();
    ts->position = {200.0f, 0.0f, 0.0f}; // visible in distance
    tradeStations.push_back(std::move(ts));
}

void World::Init(AssetManager& assets) {
    moonEnv.Init(assets);   // loads surface + tunnel models, builds colliders

    // Pre-load ship models
    assets.GetModel("assets/models/ships/player_ship.glb");
    assets.GetModel("assets/models/ships/pirate_ship.glb");
    assets.GetModel("assets/models/props/powerup_neutrino.glb");
    assets.GetModel("assets/models/environment/trade_station.glb");

    SpawnEnemies();
    SpawnPowerups();
    SpawnTradeStation();
}

void World::Update(float dt, const InputState& input, Game& game) {
    // --- Player flight ---
    FlightPhysics::Integrate(player.body, input, player.flightParams, dt);

    // --- Environment collision ---
    // Moon surface floor (Y = -5)
    const float SURFACE_Y = -5.0f;
    if (player.body.position.y < SURFACE_Y + player.boundRadius) {
        player.body.position.y = SURFACE_Y + player.boundRadius;
        if (player.body.velocity.y < 0.0f) player.body.velocity.y *= -0.15f;
    }

    // Tunnel walls: AABB clamp (simpler and more correct than sphere-sphere)
    if (moonEnv.IsInsideTunnel(player.body.position)) {
        const float HW = MoonEnvironment::TUNNEL_HALF_WIDTH - player.boundRadius;
        Vector3& pos = player.body.position;
        Vector3& vel = player.body.velocity;
        if (pos.x < -HW) { pos.x = -HW; if (vel.x < 0.0f) vel.x *= -0.15f; }
        if (pos.x >  HW) { pos.x =  HW; if (vel.x > 0.0f) vel.x *= -0.15f; }
        if (pos.y < -HW) { pos.y = -HW; if (vel.y < 0.0f) vel.y *= -0.15f; }
        if (pos.y >  HW) { pos.y =  HW; if (vel.y > 0.0f) vel.y *= -0.15f; }
    }
    player.SyncFromBody();  // re-sync after collision adjustments

    // Shield recharge
    player.shieldHitTimer -= dt;
    if (player.shieldHitTimer <= 0.0f && player.shields < 1.0f) {
        player.shields = Clamp(player.shields + player.shieldRecharge * dt, 0.0f, 1.0f);
    }

    // --- Reactor drain ---
    bool reactorDead = ReactorSystem::Update(resources, player, input.fire, dt);
    if (reactorDead) {
        game.SetState(GameState::DEAD);
        return;
    }

    // --- Enemy AI + weapons ---
    for (auto& e : enemies) {
        if (!e->active) continue;
        bool wantsToFire = EnemyAI::Update(e->ai, e->body, e->flightParams,
                                           player.position, e->health, dt);
        e->SyncFromBody();
        if (wantsToFire) {
            SpawnProjectile(e->body.position,
                            Vector3Add(Vector3Scale(e->body.forward, WeaponSystem::PROJECTILE_SPEED),
                                       e->body.velocity),
                            0.15f, e->id, false);
        }
    }

    // --- Player weapon ---
    WeaponSystem::Update(*this, input, dt);

    // --- Projectiles ---
    for (auto& p : projectiles) {
        if (p->active) p->Update(dt);
    }

    // --- Powerups animation ---
    for (auto& pu : powerups) {
        if (pu->active) pu->Update(dt);
    }

    // --- Trade station spin ---
    for (auto& ts : tradeStations) {
        ts->Update(dt);
    }

    // --- Powerup pickup ---
    PowerupSystem::Update(*this, game.Audio());

    // --- Collision resolution ---
    // Build flat entity pointer list
    std::vector<Entity*> allEntities;
    allEntities.push_back(&player);
    for (auto& e : enemies)      if (e->active) allEntities.push_back(e.get());
    for (auto& p : projectiles)  if (p->active) allEntities.push_back(p.get());
    for (auto& pu : powerups)    if (pu->active) allEntities.push_back(pu.get());
    for (auto& ts : tradeStations) allEntities.push_back(ts.get());

    auto pairs = CollisionSystem::Detect(allEntities);
    for (auto& pair : pairs) {
        Entity* a = pair.a;
        Entity* b = pair.b;

        // Enemy projectile hits player
        if (a->type == EntityType::PROJECTILE && b->type == EntityType::PLAYER) std::swap(a, b);
        if (a->type == EntityType::PLAYER && b->type == EntityType::PROJECTILE) {
            Projectile* proj = static_cast<Projectile*>(b);
            if (!proj->fromPlayer) {
                float dmg = proj->damage;
                if (player.shields > 0.0f) {
                    float shieldDmg = fminf(dmg, player.shields);
                    player.shields -= shieldDmg;
                    dmg -= shieldDmg;
                }
                player.health -= dmg;
                player.shieldHitTimer = 3.0f;
                proj->active = false;
                if (player.health <= 0.0f) {
                    game.SetState(GameState::DEAD);
                    return;
                }
            }
            continue;
        }

        // Player projectile hits enemy
        if (a->type == EntityType::PROJECTILE && b->type == EntityType::ENEMY) std::swap(a, b);
        if (a->type == EntityType::ENEMY && b->type == EntityType::PROJECTILE) {
            Projectile* proj = static_cast<Projectile*>(b);
            EnemyShip* enemy = static_cast<EnemyShip*>(a);
            if (proj->fromPlayer) {
                enemy->health -= proj->damage;
                proj->active = false;
                if (enemy->health <= 0.0f) {
                    enemy->active = false;
                    game.Particles().EmitExplosion(enemy->position, 5.0f);
                }
            }
            continue;
        }

        // Player near trade station
        if (a->type == EntityType::TRADE_STATION) std::swap(a, b);
        if (a->type == EntityType::PLAYER && b->type == EntityType::TRADE_STATION) {
            TradeStation* ts = static_cast<TradeStation*>(b);
            if (ts->IsPlayerInDockRange(player.position) && input.interact) {
                game.SetState(GameState::TRADE);
                playerDockedAt = true;
            }
        }
    }

    // Warp unlocked once player has at least one neutrino to sell
    if (resources.neutrinos >= 1) warpUnlocked = true;

    if (inSystem2) {
        // Win: reach the Alpha Station receiver beacon
        for (auto& ts : tradeStations) {
            if (ts->IsPlayerInDockRange(player.position)) {
                game.SetState(GameState::WIN);
                return;
            }
        }
    }

    PruneInactive();
}

void World::WarpToSystem2() {
    inSystem2 = true;

    // Teleport player to Alpha Station space
    player.body.position = {0.0f, 0.0f, 0.0f};
    player.body.velocity = {0.0f, 0.0f, 0.0f};
    player.SyncFromBody();

    // Clear Sol enemies and powerups — new system is empty of hostiles
    enemies.clear();
    powerups.clear();

    // Replace trade stations with the Alpha Station receiver beacon
    tradeStations.clear();
    auto receiver = std::make_unique<TradeStation>();
    receiver->position   = {0.0f, 0.0f, -80.0f};  // ahead of the player
    receiver->dockRadius = 20.0f;
    receiver->spinSpeed  = 0.6f;
    tradeStations.push_back(std::move(receiver));
}

void World::PruneInactive() {
    auto isInactive = [](const auto& ptr) { return !ptr->active; };
    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), isInactive),
                      projectiles.end());
    powerups.erase(std::remove_if(powerups.begin(), powerups.end(), isInactive),
                   powerups.end());
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), isInactive),
                  enemies.end());
}
