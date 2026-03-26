# Moon Tunnels

Space combat flight sim. Fly from the moon surface into tunnels, collect neutrinos to fuel your reactor, fight pirates, and warp to Alpha Station to win.

---

## Requirements

- CMake 3.20+
- GCC or Clang with C++17
- Git (for FetchContent to pull Raylib)
- X11 development libraries: `sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev`

---

## Build & Run (Linux)

```bash
cmake -B build
cmake --build build -- -j$(nproc)
cd build && ./flightsim
```

---

## Controls

| Action | Keyboard | Steam Deck |
|---|---|---|
| Look up / down | W / S | Left stick Y |
| Look left / right | A / D | Left stick X |
| Thrust | Left Shift | X button |
| Boost | Left Ctrl | L1 |
| Fire | Space | R1 |
| Interact / Dock | F | A |
| Pause | Escape | Start |
| Pause menu navigate | W / S | D-pad up / down |
| Pause menu confirm | Enter | A |

---

## Game Loop

1. Fly into the tunnel (ahead of you at start, marked by a cyan ring)
2. Collect neutrino canisters floating inside the tunnel
3. Exit the tunnel and fight the pirate ships
4. Fly to the trade station (ahead at ~200 units)
5. Dock with **F / A** — sell neutrinos, repair shields, refuel
6. Select **Warp to Alpha Station** in the trade menu
7. Fly to the receiver beacon — you win

Reactor fuel drains constantly. If it hits zero, it's game over.

---

## Deploy to Steam Deck

**One-time setup on the Deck** (Desktop Mode → Konsole):

```bash
passwd                              # set a password
sudo systemctl enable sshd --now    # enable SSH
```

**One-time setup on your dev machine:**

```bash
ssh-copy-id deck@steamdeck.local    # passwordless SSH
```

**Deploy + launch:**

```bash
./scripts/deploy_deck.sh -r
```

**Deploy only (no launch):**

```bash
./scripts/deploy_deck.sh
```

The script builds a Release binary optimised for the Deck's CPU (`-march=znver2`) and rsyncs only changed files, so subsequent deploys are fast.

To use a specific IP instead of the hostname:

```bash
DECK_HOST=192.168.x.x ./scripts/deploy_deck.sh -r
```

---

## Assets

Placeholder `.glb` models are generated automatically:

```bash
python3 scripts/gen_placeholder_assets.py
```

Replace files in `assets/models/` with real Blender exports when ready. Export convention: Y-up, 1 unit = 1 metre, GLB format with textures embedded.
