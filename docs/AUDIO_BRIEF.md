# Moon Tunnels — Music & SFX Brief

Hard sci-fi space combat on the lunar surface. One player ship, pirate enemies, enclosed moon tunnels, a nuclear reactor that must be kept alive. Tone: tense, isolated, industrial. Think used-universe — nothing polished, nothing heroic. The audio should feel like you are alone in deep space with a ship that might fail at any moment.

---

## Quick Reference

| # | File Path | Type | Loop | Priority |
|---|---|---|---|---|
| 1 | `assets/audio/music/space_ambient.ogg` | Music | Yes | 1 |
| 2 | `assets/audio/sfx/thruster.ogg` | SFX | Yes (per-frame) | 1 |
| 3 | `assets/audio/sfx/laser_fire.ogg` | SFX | No | 1 |
| 4 | `assets/audio/sfx/explosion.ogg` | SFX | No | 1 |
| 5 | `assets/audio/sfx/neutrino_pickup.ogg` | SFX | No | 2 |
| 6 | `assets/audio/sfx/shield_hit.ogg` | SFX | No | 2 |
| 7 | `assets/audio/sfx/dock.ogg` | SFX | No | 2 |
| 8 | `assets/audio/sfx/warp.ogg` | SFX | No | 3 |
| 9 | `assets/audio/sfx/reactor_warning.ogg` | SFX | Yes (looped alarm) | 3 |

---

## Global Delivery Rules

**Format**
- All files: **OGG Vorbis** (`.ogg`) — this is what the engine loads natively
- Music: 44.1 kHz, stereo, quality ~6 (~192 kbps equivalent)
- SFX: 44.1 kHz, mono (engine handles stereo panning if needed), quality ~5

**Levels**
- Normalise all SFX to **−3 dBFS** peak
- Music: normalise to **−6 dBFS** peak (leaves headroom for SFX on top)
- No limiting or heavy compression — the engine does volume scaling at runtime

**Loops**
- Files marked "Yes" for Loop must have seamless loop points — no click at the end of the file
- Deliver loop files trimmed to exactly one loop cycle with no leading or trailing silence

**Naming**
- File names must match exactly (lowercase, underscores, `.ogg`)
- Drop into `assets/audio/music/` or `assets/audio/sfx/` as indicated

---

## Music

### 1. Space Ambient — `assets/audio/music/space_ambient.ogg`

**Role:** Background music. Starts playing at main menu and loops continuously through gameplay. There is currently no music system for dynamic layers or combat stingers — one track covers everything.

**Duration:** Minimum 3 minutes before the loop point. Longer is better.

**Mood:** Vast, cold, uneasy. The player is alone on an airless moon with a reactor that is slowly draining. It should feel like a ticking clock buried under silence.

**Direction:**
- Sparse — more space than sound. Long reverb tails, slow harmonic movement.
- Pads and drones as the foundation. Sub-bass undertone (suggest reactor hum).
- Occasional textural events: metallic scrapes, distant radio static, granular noise sweeps. These should feel environmental, not musical.
- No melody, no rhythm section, no drums. Definitely no heroics.
- Reference zone: dark ambient, sci-fi industrial underscore. Think Ennio Morricone's *The Thing* score crossed with Brian Eno's *Apollo*.
- Should fade into the background during combat without competing with SFX.

**Loop:** Seamless. The loop point should land on a quiet moment in the pads.

---

## SFX

### 2. Thruster — `assets/audio/sfx/thruster.ogg`

**Role:** Plays every frame the player is thrusting (Left Shift / X button). Volume scales with thrust input (0 to 0.7 × thrust). The engine re-triggers the sound each frame it isn't already playing, so the file acts as a continuous loop.

**Duration:** 1–2 seconds per cycle, seamless loop.

**Direction:**
- Plasma or ion drive hum — sustained, layered, slightly distorted.
- Low-frequency rumble with a mid-range harmonic buzz. No rocket exhaust roar (this is space; the sound is internal vibration coming through the hull, not air noise).
- Should swell naturally as volume increases — the harmonic content should feel louder, not just the level.
- Subtle pitch variation acceptable (±2–3 semitones through the loop adds life).

**Loop:** Seamless — this is the most critical loop in the game.

---

### 3. Laser Fire — `assets/audio/sfx/laser_fire.ogg`

**Role:** Plays each time the player fires (max 5 shots/second). Also used for enemy fire — same sound for both (enemies fire red projectiles, player fires yellow; the SFX does not distinguish).

**Duration:** 0.1–0.25 seconds. Short, punchy.

**Direction:**
- Photon pulse / particle beam — a brief high-pitched crack or zap, not a slow "pew".
- Slight tail of about 80–120 ms of harmonic decay. Clean transient at the front.
- Should cut cleanly through the ambient music without feeling cartoonish.
- Not a big cinematic blast — this is a small fighter with a light cannon.

---

### 4. Explosion — `assets/audio/sfx/explosion.ogg`

**Role:** Plays when an enemy ship is destroyed. Spatial — volume scales with distance from the player (engine uses `PlaySpatial` with a max distance of ~100 m).

**Duration:** 1.0–2.0 seconds including tail.

**Direction:**
- In space: no atmosphere, so the explosion is felt through the hull as a structural shockwave before fading.
- Start: hard low-frequency impact (the shockwave). Short (~30 ms) subsonic thump.
- Body: debris scatter — metallic fragments, crackling electrical discharge.
- Tail: 0.5–1 s of dissipating rumble, filtering to silence.
- No big cinematic boom — this is a small ship, not a capital vessel.

---

### 5. Neutrino Pickup — `assets/audio/sfx/neutrino_pickup.ogg`

**Role:** Plays when the player flies through a neutrino canister and collects it. UI sound — full volume regardless of distance.

**Duration:** 0.4–0.8 seconds.

**Direction:**
- A positive confirmation — reactor is being fed.
- Energetic, synthetic. Rising tone or chord that resolves upward.
- Slightly glowing, slightly alien — not a video-game coin sound, but in that emotional territory.
- Should feel like relief: the reactor got a meal.

---

### 6. Shield Hit — `assets/audio/sfx/shield_hit.ogg`

**Role:** Plays when an enemy projectile hits the player's shields. UI sound (not spatial). Shields recharge after 3 seconds without being hit.

**Duration:** 0.3–0.5 seconds.

**Direction:**
- Energy field absorbing impact — an electric crack or plasma discharge.
- Distinct from the laser fire sound: where laser fire is a sharp attack, this should feel like a deflection — a brief buzz or crackle that says "that hit, but you're still here".
- Urgency without panic.

---

### 7. Dock — `assets/audio/sfx/dock.ogg`

**Role:** Plays when the player docks with the trade station (F / A button within 15 m). UI sound.

**Duration:** 0.5–1.0 seconds.

**Direction:**
- Mechanical connection — a solid clunk of docking clamps engaging, followed by a pressurisation hiss or a confirmation tone.
- Should feel like a moment of safety: you made it to the station.
- Can be two-part: impact clunk → brief tone or breath of air.

---

### 8. Warp — `assets/audio/sfx/warp.ogg`

**Role:** Plays once when the player activates warp to Alpha Station (key 4 in trade menu). This is a one-time per-run event — the most dramatic sound moment in the game. UI sound.

**Duration:** 2.0–4.0 seconds.

**Direction:**
- Space compression / FTL jump. Should feel like physics bending.
- Build: rising harmonic sweep or pitch-up, 0.5–1 s.
- Peak: hard transient crack — the jump itself.
- Tail: trailing echo / reverb wash, the other side of the warp.
- This is the one moment where the audio can be bigger and more cinematic than the rest of the game. The player just survived the moon run — reward them.

---

### 9. Reactor Warning — `assets/audio/sfx/reactor_warning.ogg`

**Role:** Alarm sound for low reactor fuel. Not yet wired in the game code but the slot is reserved — it will loop when fuel drops below ~20%. The engine will handle loop triggering.

**Duration:** 0.5–1.5 seconds, seamless loop.

**Direction:**
- Klaxon or pulse alarm — the kind of sound that demands attention without being so annoying the player mutes it.
- Slow pulse: 1–2 beeps per second. Low-mid frequency so it doesn't clash with the thruster hum.
- Slightly degraded or distorted — the reactor is failing, the alarm sounds like the ship is struggling.
- Should create tension without becoming background noise.

**Loop:** Seamless. Triggered and stopped by the engine — the file just needs to loop cleanly.
