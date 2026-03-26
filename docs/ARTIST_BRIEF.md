# Moon Tunnels — 3D Artist Brief

Space combat flight sim on the lunar surface. The player pilots a fighter into moon tunnels to collect neutrino fuel, fights pirate ships, then warps to a distant trade station. Tone: hard sci-fi, utilitarian military hardware, weathered surfaces. No fantasy, no cartoon — think used-universe.

---

## Quick Reference

| Asset | File Path | Priority | Collision | Poly Budget |
|---|---|---|---|---|
| Player Ship | `assets/models/ships/player_ship.glb` | 1 | 2.5 m radius sphere | 5 000 tris |
| Pirate Ship | `assets/models/ships/pirate_ship.glb` | 1 | 2.5 m radius sphere | 5 000 tris |
| Trade Station | `assets/models/environment/trade_station.glb` | 2 | 12 m radius sphere | 20 000 tris |
| Tunnel Section A | `assets/models/environment/tunnel_section_a.glb` | 2 | AABB 20×20×115 m | 20 000 tris |
| Moon Surface | `assets/models/environment/moon_surface.glb` | 3 | Floor plane at Y = 0 | 20 000 tris |
| Neutrino Canister | `assets/models/props/powerup_neutrino.glb` | 3 | 1.5 m radius sphere | 1 000 tris |

---

## Global Export Rules

These rules apply to every asset. Deviating from them will break the game.

**Format**
- Export as **GLB** (binary glTF 2.0)
- Embed all textures inside the GLB — do not ship separate PNG/JPG files
- File names must match exactly: lowercase, underscores, `.glb` extension

**Scale**
- **1 Blender unit = 1 metre** — apply all transforms before export (`Ctrl+A → All Transforms`)
- The engine draws every model at scale 1.0 with no correction; wrong scale cannot be fixed in code

**Axes**
- Use Blender's default glTF export settings: **Y-up, −Z-forward**
- Blender → glTF conversion handles the axis swap automatically — do not rotate models to compensate

**Materials**
- **PBR metallic-roughness** only (Principled BSDF in Blender maps correctly)
- No Cycles-only nodes, no geometry nodes, no shader node groups
- Emissive channel is supported and encouraged for glowing elements
- One material per mesh is preferred; two materials maximum per asset

**Geometry**
- Apply all modifiers before export
- No armatures, no shape keys, no animations — the engine handles all motion in code
- Triangulate before export or enable "Triangulate" in the glTF export dialog
- Smooth normals where appropriate; hard edges on panel lines

**Delivery**
- Drop the finished `.glb` into the correct `assets/models/` subdirectory
- The build system copies assets automatically — no manual step needed
- Test by rebuilding (`cmake --build build`) and launching the game

---

## Asset 1 — Player Ship

**File:** `assets/models/ships/player_ship.glb`
**Priority:** 1 (most player-facing asset)

### In-Game Role
The ship the player flies for the entire game. Seen constantly from behind via a chase camera positioned 14 m back and 3.5 m above. The rear silhouette and engine glow are what the player looks at most.

### Dimensions & Constraints
- **Must fit inside a 5 m diameter sphere** (2.5 m radius). This is the collision boundary — nothing should protrude past it.
- Approximate proportions from placeholder: fuselage ~6 m long, ~1 m wide, ~0.5 m tall; wingspan ~6 m tip to tip.
- Forward direction is **−Z** in model space. Nose points toward −Z.
- The engine fires along +Z (backward). Thruster particle effects emit from ~2.5 m behind the origin.

### Design Direction
- Single-seat interceptor / light fighter. Nimble, purpose-built.
- Swept wings, prominent cockpit canopy, twin engine nozzles at the rear.
- Colour palette: **cool blue-grey** primary hull, accent lighting in cyan. Suggests a corporate or military-issue craft that has seen hard use.
- Panel lines, heat staining around engine nozzles, sensor arrays on the nose.
- Cockpit glass: semi-opaque or tinted — the player character is never shown.

### Delivery Checklist
- [ ] Fits inside 2.5 m radius sphere from origin
- [ ] Nose points toward −Z
- [ ] All transforms applied
- [ ] GLB with embedded textures
- [ ] PBR materials only
- [ ] Under 5 000 triangles

---

## Asset 2 — Pirate Ship

**File:** `assets/models/ships/pirate_ship.glb`
**Priority:** 1

### In-Game Role
Enemy AI fighters. Up to 3 on screen simultaneously. They patrol, chase the player, and fire red projectiles. Seen from various angles during combat.

### Dimensions & Constraints
- **Must fit inside a 5 m diameter sphere** (2.5 m radius) — same as the player ship.
- Approximate proportions: fuselage ~5 m long, ~1.1 m wide, ~0.7 m tall; wingspan shorter and wider than player's.
- Forward direction is **−Z** in model space.
- Has a weapon mount on top (turret bump or gun housing) — purely decorative, the engine draws projectile spheres in code.

### Design Direction
- Heavy corvette / gunboat. Slower and more threatening than the player ship.
- Blocky, aggressive silhouette. More armour plating, less aerodynamic refinement.
- Must read as "enemy" at a glance in combat — distinct from the player ship in shape language.
- Colour palette: **dark red / maroon** primary hull, rust and carbon scoring. Suggests salvaged or pirated military surplus.
- Asymmetrical details acceptable (patched hull plates, mismatched components).

### Delivery Checklist
- [ ] Fits inside 2.5 m radius sphere from origin
- [ ] Visually distinct from player ship (different silhouette, different colour)
- [ ] Nose points toward −Z
- [ ] All transforms applied
- [ ] GLB with embedded textures
- [ ] Under 5 000 triangles

---

## Asset 3 — Trade Station

**File:** `assets/models/environment/trade_station.glb`
**Priority:** 2

### In-Game Role
A space station the player docks with to sell neutrinos, buy repairs, and unlock warp. Appears in both the Sol system (at 200 m distance) and the Alpha Station system (at 80 m). The engine draws a docking range indicator ring (cyan, 15 m radius) when the player is nearby.

### Dimensions & Constraints
- **Must fit inside a 24 m diameter sphere** (12 m radius from origin). The collision system uses this radius.
- The engine spins the station on its Y-axis continuously (0.3 rad/s in Sol, 0.6 rad/s at Alpha). Symmetric or rotationally balanced designs work best — asymmetric detail is fine but nothing should look wrong mid-spin.
- Docking approach is from any direction; the station should read clearly from 80–200 m.

### Design Direction
- Torus ring station with a central hub and radial spokes. Roughly 16–20 m outer diameter.
- Utilitarian: solar panels, docking ports, antenna arrays, cargo containers docked at the ring.
- Colour palette: **gold / pale tan** primary hull with white and grey panels. Lighting strips in amber or warm white.
- Should feel like a working commercial outpost, not military. Welcoming relative to the pirate ships.
- Landing pad or docking bay visible on the ring (purely decorative — docking is trigger-based).

### Delivery Checklist
- [ ] Entire model fits inside 12 m radius sphere
- [ ] Looks correct when spinning on Y-axis
- [ ] Readable silhouette from 80+ m distance
- [ ] All transforms applied
- [ ] GLB with embedded textures
- [ ] Under 20 000 triangles

---

## Asset 4 — Tunnel Section A

**File:** `assets/models/environment/tunnel_section_a.glb`
**Priority:** 2

### In-Game Role
The enclosed passage bored through the moon. The player enters at Z = −15, flies to Z = −130, collecting neutrino canisters floating inside. The walls are hard — the engine AABB-clamps the player inside. Three enemy collision spheres also live along the walls.

### Dimensions & Constraints — HARD LIMITS
- **Inner cross-section: exactly 20 × 20 m.** The collision system clamps the player to ±9 m on X and Y from the tunnel centreline. Walls must not protrude past ±10 m. Interior detail can come up to ±9.5 m at most.
- **Length: 115 m** along the Z-axis (entrance at Z = −15, exit at Z = −130 in world space). The model is placed at the midpoint (0, 0, −72.5), so the mesh should span −57.5 m to +57.5 m on Z in model space.
- The engine draws a **cyan entrance ring at R = 10.8 m** at the mouth (world Z = −15). The tunnel opening should match this diameter (~21.6 m).
- No end caps — the tunnel is open at both ends.

### Design Direction
- Square cross-section (hard constraint). Can chamfer the inner corners slightly for visual interest, but keep the flat walls as the dominant surface.
- Interior: industrial moon-boring machine cuts — raw rock walls with metal reinforcement ribs every 8–12 m, cable runs, emergency lighting strips near the floor.
- Colour palette: **dark grey rock** with charcoal metal reinforcement, amber safety lighting baked into the texture.
- The three neutrino canisters float at Z = −40, −70, −100 inside the tunnel — plan lighting or environment detail to make these areas feel like staging points.
- Exterior (visible from moon surface): a rough circular bore mouth in rock, no ornamentation.

### Delivery Checklist
- [ ] Inner walls sit at exactly ±10 m on X and Y (20 m interior span)
- [ ] Mesh spans 115 m on Z in model space (centred at origin)
- [ ] Tunnel mouth diameter ~21.6 m to match engine entrance ring
- [ ] No end caps
- [ ] All transforms applied
- [ ] GLB with embedded textures
- [ ] Under 20 000 triangles

---

## Asset 5 — Moon Surface

**File:** `assets/models/environment/moon_surface.glb`
**Priority:** 3

### In-Game Role
The static ground plane. The player starts above it (Y = 5) and the game prevents falling below Y = −5. It is always the first thing rendered. The tunnel entrance is at world position (0, 0, −15) and should appear as a hole/opening in the surface.

### Dimensions & Constraints
- The engine renders the model at world position **(0, −5, 0)**. The top surface of your mesh should sit at approximately **Y = 0** in model space (which puts it at Y = −5 in world space — the hard floor).
- Cover at least **400 × 400 m** on X and Z. The engine falls back to a 600 m plane if the model is missing, so match or exceed that footprint.
- Keep the surface relatively flat: **no peaks above Y = 2** in model space (world Y = −3). The player ship starts at world Y = 5, and sharp terrain would cause clipping.
- A cave/bore mouth at approximately **(0, 0, −15)** in model space would tie the surface to the tunnel entrance. It should be a rough circular opening ~22 m in diameter, matching the tunnel mouth.

### Design Direction
- Airless lunar regolith — pale grey, fine dust, ancient impact craters.
- Colour palette: **mid grey** (0.55–0.65 value) with subtle blue-grey shadowing. Low specularity (roughness 0.9+).
- Craters of varying sizes, loose boulder fields, ridge lines in the distance.
- No vegetation, no water, no atmosphere effects.
- The tunnel entrance should look like an industrial bore operation — metal frame around the mouth, warning lights (baked), debris pile at the edge.

### Delivery Checklist
- [ ] Top surface near Y = 0 in model space
- [ ] Footprint at least 400 × 400 m
- [ ] No geometry above Y = 2 in model space
- [ ] Tunnel bore mouth at (0, 0, −15) approximately 22 m diameter
- [ ] All transforms applied
- [ ] GLB with embedded textures
- [ ] Under 20 000 triangles

---

## Asset 6 — Neutrino Canister

**File:** `assets/models/props/powerup_neutrino.glb`
**Priority:** 3

### In-Game Role
Collectible fuel containers floating inside the tunnel. Three placed at tunnel depths −40, −70, −100. The engine bobs each canister ±0.8 m on Y (1.5 Hz sine wave) and spins it on Y-axis (0.8 rad/s). Players fly through them to collect; a 1.5 m radius sphere triggers pickup.

### Dimensions & Constraints
- **Must fit inside a 3 m diameter sphere** (1.5 m radius). Anything outside this sphere will visually clip through tunnel walls before the pickup triggers.
- Design should be **symmetric on the Y-axis** (rotation axis for spin). It doesn't need to be symmetric on X/Z.
- Vertical extent: keep within ~1.0 m tall to feel natural while bobbing.

### Design Direction
- A pressurised reactor fuel cell or isotope container — something a sci-fi refinery would produce.
- Cylindrical or octagonal prism with end caps. Indicator lights (emissive) showing charge level.
- Colour palette: **glowing cyan** (emissive channel strongly recommended) on a dark housing — gunmetal or matte black chassis with bright blue-white glow strips.
- Should be eye-catching from 20–30 m in a dark tunnel.
- Handle grips or mounting flanges on the sides add readability.

### Delivery Checklist
- [ ] Fits inside 1.5 m radius sphere from origin
- [ ] Symmetric on Y-axis (looks correct while spinning)
- [ ] Emissive material for glow effect
- [ ] All transforms applied
- [ ] GLB with embedded textures
- [ ] Under 1 000 triangles

---

## Handoff & Testing

1. Drop finished `.glb` files into the matching directory under `assets/models/`
2. Rebuild the project: `cmake --build build`  — assets are copied automatically
3. Launch the game: `cd build && ./flightsim`
4. The game loads models on startup; check the terminal for any load errors
5. If a model fails to load the engine falls back to a primitive shape — this means the file path or format is wrong

**Blender export settings summary:**
- File > Export > glTF 2.0 (.glb)
- Format: glTF Binary (.glb)
- Include: Selected Objects or Scene
- Geometry: Apply Modifiers ON, Triangulate ON
- Data > Mesh: Normals ON
- Materials: Export ON, Images: Automatically detect (embed)
- Transform: Y Up ON (default)
