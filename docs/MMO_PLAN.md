# Moon Tunnels — MMO Transition Plan

Moon Tunnels ships as a single-player game. After release, the goal is to evolve it into an MMO: multiple players sharing the same solar system, seeing each other's ships, competing or cooperating for neutrinos, fighting pirates, and trading. This document identifies the architectural work required, the technology choices, and the phased approach to get there without a full rewrite.

---

## Current Architecture — What's Already MMO-Friendly

| Component | Readiness | Notes |
|---|---|---|
| `FlightPhysics::Integrate` | **Excellent** | Pure stateless function — runs identically on server and client |
| `InputState` struct | **Excellent** | Clean 10-field struct, can be serialised and sent over the wire as-is |
| Entity base class | **Good** | Has `EntityId`, position, rotation, boundRadius — maps directly to a network entity |
| Static systems (ReactorSystem, WeaponSystem) | **Good** | Functional design; server runs same code, no UI coupling |
| Asset pipeline | **Neutral** | Server doesn't need assets; client loads as now |
| `World ↔ Game` coupling | **Problem** | `World::Update` takes `Game&` and calls back into it — must decouple first |
| Collision detection | **Problem** | O(n²) fine for <100 entities; needs spatial partitioning for many players |
| Persistence | **Missing** | No save system; credits/neutrinos are ephemeral |
| Networking | **Missing** | Must build from scratch |

---

## Technology Choices

### Networking — ENet
- UDP-based with optional per-packet reliability (unreliable, reliable-unordered, reliable-ordered)
- C library, links directly into the C++ project with zero framework overhead
- Installs via FetchContent — same pattern already used for Raylib
- Battle-tested in games (Cube 2, many indie titles)
- _Alternative considered_: Valve's GameNetworkingSockets — more features but a much heavier dependency

### Server
- Dedicated **headless C++ server binary** built from the same codebase
- `MOON_TUNNELS_SERVER` compile flag strips Raylib, audio, and all rendering
- Runs the same `World::Update` tick loop at **30 Hz**; clients render at 60 Hz with interpolation
- No GUI, no asset loading — physics, AI, and collision only

### Persistence — SQLite → PostgreSQL
- **Phase 1**: SQLite — single file, zero config, sufficient for hundreds of concurrent players
- **Later**: migrate to PostgreSQL if concurrent writes or player count demands it
- Schema: accounts (uuid, username, password hash), player_state (credits, neutrinos, ships unlocked)

### Authentication
- Server issues a session token on login
- Client sends token on every connect
- No OAuth at launch scale — simple challenge/response is enough

---

## Architecture Changes Required

### 1. Decouple World from Game _(prerequisite for everything else)_

**Problem**: `World::Update(float dt, InputState, Game& game)` calls `game.SetState()`, `game.Particles()`, and `game.Audio()` directly.

**Fix**: Replace the `Game&` parameter with a lightweight event queue. World posts `GameEvent` structs (StateChange, EmitExplosion, PlaySound). The client `Game` loop consumes them each frame. The headless server simply discards audio and particle events.

Files: `src/world/World.h`, `src/world/World.cpp`, `src/core/Game.h`, `src/core/Game.cpp`, new `src/core/GameEvent.h`

### 2. Split client and server build targets

Add `MOON_TUNNELS_SERVER` flag to `CMakeLists.txt`. Server target links no Raylib, no miniaudio — just physics, AI, ENet, and SQLite.

Files: `CMakeLists.txt`

### 3. Replace singleton PlayerShip with a player map

`World` currently has one `PlayerShip player`. For MMO it becomes:
```cpp
std::unordered_map<EntityId, PlayerShip> players;
EntityId localPlayerId; // client only
```
The local player is identified by their session EntityId. All rendering and HUD code uses `players.at(localPlayerId)` where it currently uses `player`.

Files: `src/world/World.h`, `src/world/World.cpp`, `src/rendering/Renderer.cpp`, `src/core/Game.cpp`

### 4. Add NetworkManager

New system: `src/network/NetworkManager.h/.cpp`

- **Client**: connect to server, send `InputPacket` every frame, receive `WorldSnapshot` and feed interpolation buffer
- **Server**: accept connections, assign EntityIds, run authoritative tick, broadcast `WorldSnapshot` at 20 Hz

### 5. Client-side prediction

The client runs `FlightPhysics::Integrate` locally on its own player immediately on input — no waiting for the server. The server sends authoritative positions; the client reconciles (smooth correction or snap depending on error magnitude). This is non-negotiable for flight controls to feel responsive over latency.

### 6. State snapshots

Server sends a `WorldSnapshot` at 20 Hz. Clients interpolate between the two most recently received snapshots for smooth rendering at 60 Hz.

Snapshot contents:
- All active player positions, velocities, orientations, health, shields
- All enemy positions, health, AI state tag
- Active powerup entity IDs and positions
- Active projectile positions and velocities

---

## Network Packet Design

```cpp
// Client → Server  (~60 Hz, unreliable UDP)
struct InputPacket {
    uint32_t sessionToken;
    uint32_t sequenceNum;   // used by server for reconciliation
    float    thrust, pitch, yaw, roll;
    uint8_t  flags;         // fire | boost | interact packed as bits
};

// Per-player state inside a snapshot
struct PlayerNetState {
    EntityId id;
    Vector3  position;
    Vector3  velocity;
    float    pitchAngle, yawAngle;  // compact rotation (no roll needed for MMO)
    float    health, shields;
};

// Server → All Clients  (20 Hz, reliable-unordered)
struct WorldSnapshot {
    uint32_t      serverTick;
    uint8_t       playerCount;
    PlayerNetState players[64];     // up to 64 players per zone
    // variable-length arrays for enemies, projectiles, powerups follow
};
```

Projectiles and fast-moving entities are sent at 60 Hz unreliable to minimise lag on hits. Slow-changing state (trade prices, warp flags) is sent reliable-ordered only on change.

---

## Phased Implementation

### Phase A — Foundation
_Goal: Two clients connect and see each other's ships move._

1. Decouple `World` from `Game` via event queue (`GameEvent.h`)
2. Add `MOON_TUNNELS_SERVER` CMake target (compiles headless, no Raylib)
3. Integrate ENet via FetchContent
4. Replace `PlayerShip player` with `unordered_map<EntityId, PlayerShip> players`
5. Minimal server: accepts connections, assigns EntityIds, echoes positions
6. Client: connects, sends `InputPacket`, renders remote player ships

**Checkpoint**: Two `./flightsim` clients connect to `./server`; both see each other moving in real time.

---

### Phase B — Authoritative Server
_Goal: Server owns all game logic. Clients are thin renderers + input senders._

1. Server runs full `World::Update` at 30 Hz for all players
2. Server spawns and controls all enemies via EnemyAI
3. Server authoritative on: projectile hit detection, powerup pickup, death
4. Client sends `InputPacket`; server applies to that player's `PlayerShip`
5. Server broadcasts `WorldSnapshot`; clients render all entities
6. Client-side prediction for local player flight (reconcile with server positions)

**Checkpoint**: Two players fight pirates together; a powerup is collected by exactly one player; kills appear on both screens simultaneously.

---

### Phase C — Persistence and Accounts
_Goal: Player progress survives between sessions._

1. SQLite schema: `accounts`, `player_state` tables
2. Login handshake on connect (username + password hash → session token)
3. Server loads player state on connect, saves on trade/disconnect
4. Credits and neutrinos persist across sessions
5. Respawn policy: always spawn on moon surface (no position persistence for now)

**Checkpoint**: Sell neutrinos, quit, reconnect — credits are unchanged.

---

### Phase D — Scale and Anti-Cheat
_Goal: Support 50+ concurrent players without cheating._

1. **Interest management**: server only sends entities within N units of each player (reduces bandwidth linearly with player count)
2. **Spatial partitioning**: replace O(n²) collision with a uniform grid or BVH
3. **Zone sharding**: separate server instances per solar system region if needed
4. **Server-side validation**: verify all pickups, damage events, and trades — reject impossible inputs (position teleport, negative credits, etc.)
5. **Rate limiting**: cap input packets per client; kick on flood

**Checkpoint**: 20+ clients connected; server tick completes in under 33 ms.

---

## New Files

| File | Purpose |
|---|---|
| `src/network/NetworkManager.h/.cpp` | ENet wrapper — send/receive, connection lifecycle |
| `src/network/Packets.h` | All packet struct definitions (POD, no pointers) |
| `src/server/ServerMain.cpp` | Headless server entry point, tick loop |
| `src/core/GameEvent.h` | Event types posted by World, consumed by Game or discarded by server |
| `db/schema.sql` | SQLite schema for accounts and player state |

## Modified Files

| File | Change |
|---|---|
| `CMakeLists.txt` | Add `server` target, ENet FetchContent |
| `src/world/World.h/.cpp` | Player map, event queue, remove `Game&` from Update signature |
| `src/core/Game.h/.cpp` | Consume event queue, hold NetworkManager |
| `src/rendering/Renderer.cpp` | Render all players in map, not just singleton |
| `src/rendering/HUD.h` | Add remote player positions to radar blips |

---

## Verification Milestones

| Phase | Test |
|---|---|
| A | Two clients connect, both ships visible and moving smoothly |
| B | Pirate killed on one client disappears on the other within one server tick |
| C | Trade, disconnect, reconnect — credits match |
| D | 20 clients connected, `top` shows server CPU < 10%, no tick overruns |
