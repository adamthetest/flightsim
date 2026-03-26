#!/usr/bin/env python3
"""
Generates placeholder .glb assets for all game models using pygltflib + numpy.
No Blender needed — produces recognizable low-poly geometry for each role.

Usage: python3 scripts/gen_placeholder_assets.py

Requirements: pip install pygltflib numpy
"""

import struct
import os
import numpy as np
from pygltflib import (
    GLTF2, Scene, Node, Mesh, Primitive, Accessor, BufferView, Buffer,
    Material, PbrMetallicRoughness, Asset,
    FLOAT, UNSIGNED_INT, ARRAY_BUFFER, ELEMENT_ARRAY_BUFFER,
    TRIANGLES,
)

REPO_ROOT = os.path.join(os.path.dirname(__file__), "..")


# ---------------------------------------------------------------------------
# Low-level helpers
# ---------------------------------------------------------------------------

def pack_mesh(positions: np.ndarray, indices: np.ndarray,
              color_rgba=(0.6, 0.7, 0.8, 1.0)) -> GLTF2:
    """Build a minimal GLTF2 document from float32 positions and uint32 indices."""
    positions = positions.astype(np.float32)
    indices   = indices.astype(np.uint32)

    pos_bytes = positions.tobytes()
    idx_bytes = indices.tobytes()

    # Align idx start to 4-byte boundary
    padding = (4 - len(pos_bytes) % 4) % 4
    blob = pos_bytes + b'\x00' * padding + idx_bytes

    gltf = GLTF2(
        asset=Asset(version="2.0", generator="moon-tunnels-gen"),
        scene=0,
        scenes=[Scene(nodes=[0])],
        nodes=[Node(mesh=0)],
        meshes=[Mesh(primitives=[Primitive(
            attributes={"POSITION": 0},
            indices=1,
            material=0,
            mode=TRIANGLES,
        )])],
        accessors=[
            # 0: positions
            Accessor(
                bufferView=0,
                byteOffset=0,
                componentType=FLOAT,
                count=len(positions),
                type="VEC3",
                min=positions.min(axis=0).tolist(),
                max=positions.max(axis=0).tolist(),
            ),
            # 1: indices
            Accessor(
                bufferView=1,
                byteOffset=0,
                componentType=UNSIGNED_INT,
                count=len(indices),
                type="SCALAR",
            ),
        ],
        bufferViews=[
            BufferView(buffer=0, byteOffset=0,
                       byteLength=len(pos_bytes), target=ARRAY_BUFFER),
            BufferView(buffer=0, byteOffset=len(pos_bytes) + padding,
                       byteLength=len(idx_bytes), target=ELEMENT_ARRAY_BUFFER),
        ],
        buffers=[Buffer(byteLength=len(blob))],
        materials=[Material(
            pbrMetallicRoughness=PbrMetallicRoughness(
                baseColorFactor=list(color_rgba),
                metallicFactor=0.6,
                roughnessFactor=0.4,
            ),
            doubleSided=True,
        )],
    )
    gltf.set_binary_blob(blob)
    return gltf


def save(gltf: GLTF2, path: str):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    gltf.save_binary(path)
    size = os.path.getsize(path)
    print(f"  wrote {path}  ({size} bytes)")


# ---------------------------------------------------------------------------
# Geometry builders
# ---------------------------------------------------------------------------

def box(w, h, d):
    """Axis-aligned box centered at origin."""
    x, y, z = w/2, h/2, d/2
    v = np.array([
        [-x,-y,-z], [ x,-y,-z], [ x, y,-z], [-x, y,-z],  # back
        [-x,-y, z], [ x,-y, z], [ x, y, z], [-x, y, z],  # front
    ], dtype=np.float32)
    i = np.array([
        0,1,2, 0,2,3,   # back
        4,6,5, 4,7,6,   # front
        0,4,5, 0,5,1,   # bottom
        2,6,7, 2,7,3,   # top
        0,3,7, 0,7,4,   # left
        1,5,6, 1,6,2,   # right
    ], dtype=np.uint32)
    return v, i


def merge(*meshes):
    """Concatenate (positions, indices) pairs into one mesh."""
    all_pos = []
    all_idx = []
    offset = 0
    for pos, idx in meshes:
        all_pos.append(pos)
        all_idx.append(idx + offset)
        offset += len(pos)
    return np.vstack(all_pos), np.concatenate(all_idx)


def translate(pos, dx=0, dy=0, dz=0):
    return pos + np.array([dx, dy, dz], dtype=np.float32)


def scale_mesh(pos, sx=1, sy=1, sz=1):
    return pos * np.array([sx, sy, sz], dtype=np.float32)


# ---------------------------------------------------------------------------
# Individual model builders
# ---------------------------------------------------------------------------

def make_player_ship():
    """Delta-wing fighter: fuselage cylinder approximation + swept wings + cockpit."""
    # Central fuselage: narrow box, elongated on Z
    fuse_v, fuse_i = box(0.8, 0.5, 6.0)
    fuse_v = translate(fuse_v, 0, 0.1, 0)

    # Nose cone (4-sided pyramid pointing forward, -Z is forward in Raylib)
    nose_v = np.array([
        [ 0.0,  0.0, -4.0],   # tip
        [-0.4, -0.25, -2.5],
        [ 0.4, -0.25, -2.5],
        [ 0.4,  0.25, -2.5],
        [-0.4,  0.25, -2.5],
    ], dtype=np.float32)
    nose_i = np.array([
        0,2,1, 0,3,2, 0,4,3, 0,1,4,   # 4 faces of pyramid
        1,2,3, 1,3,4,                  # base quad
    ], dtype=np.uint32)

    # Left wing (swept back, flat triangle slab)
    lw_v = np.array([
        [-0.4,  0.0, -1.5],
        [-3.0,  0.0,  1.0],
        [-0.4,  0.0,  2.0],
        [-0.4,  0.15,-1.5],
        [-3.0,  0.15, 1.0],
        [-0.4,  0.15, 2.0],
    ], dtype=np.float32)
    lw_i = np.array([
        0,2,1, 3,4,5,      # top/bottom
        0,1,4, 0,4,3,      # front edge
        1,2,5, 1,5,4,      # back edge
        0,3,5, 0,5,2,      # root edge
    ], dtype=np.uint32)

    # Right wing (mirror)
    rw_v = lw_v.copy(); rw_v[:,0] *= -1
    rw_i = lw_i.copy()

    # Cockpit bubble
    cp_v, cp_i = box(0.45, 0.3, 0.7)
    cp_v = translate(cp_v, 0, 0.4, -1.5)

    # Engine pods
    lp_v, lp_i = box(0.3, 0.3, 1.2)
    lp_v = translate(lp_v, -0.7, -0.1, 1.5)
    rp_v, rp_i = box(0.3, 0.3, 1.2)
    rp_v = translate(rp_v,  0.7, -0.1, 1.5)

    return merge(
        (fuse_v, fuse_i), (nose_v, nose_i),
        (lw_v, lw_i), (rw_v, rw_i),
        (cp_v, cp_i),
        (lp_v, lp_i), (rp_v, rp_i),
    )


def make_pirate_ship():
    """Chunkier, asymmetric-looking ship for the enemy."""
    # Heavy fuselage
    fuse_v, fuse_i = box(1.1, 0.7, 5.0)

    # Blunt nose
    nose_v = np.array([
        [ 0.0,  0.0, -3.2],
        [-0.55,-0.35,-2.0],
        [ 0.55,-0.35,-2.0],
        [ 0.55, 0.35,-2.0],
        [-0.55, 0.35,-2.0],
    ], dtype=np.float32)
    nose_i = np.array([
        0,2,1, 0,3,2, 0,4,3, 0,1,4,
        1,2,3, 1,3,4,
    ], dtype=np.uint32)

    # Stubby wings
    lw_v = np.array([
        [-0.55, 0.0, -0.5],
        [-2.5,  0.0,  0.5],
        [-0.55, 0.0,  1.5],
        [-0.55, 0.2, -0.5],
        [-2.5,  0.2,  0.5],
        [-0.55, 0.2,  1.5],
    ], dtype=np.float32)
    lw_i = np.array([
        0,2,1, 3,4,5,
        0,1,4, 0,4,3,
        1,2,5, 1,5,4,
        0,3,5, 0,5,2,
    ], dtype=np.uint32)
    rw_v = lw_v.copy(); rw_v[:,0] *= -1
    rw_i = lw_i.copy()

    # Big engine block at rear
    eng_v, eng_i = box(0.9, 0.6, 1.5)
    eng_v = translate(eng_v, 0, -0.05, 2.0)

    # Turret bump on top
    turr_v, turr_i = box(0.4, 0.35, 0.6)
    turr_v = translate(turr_v, 0, 0.52, -0.5)

    return merge(
        (fuse_v, fuse_i), (nose_v, nose_i),
        (lw_v, lw_i), (rw_v, rw_i),
        (eng_v, eng_i), (turr_v, turr_i),
    )


def make_neutrino_canister():
    """8-sided prism (cylinder approximation) with end caps — a sci-fi canister."""
    N = 8
    R = 0.6
    H = 1.0
    angles = np.linspace(0, 2*np.pi, N, endpoint=False)
    rim = np.column_stack([np.cos(angles)*R, np.sin(angles)*R])

    # Bottom ring, top ring, bottom center, top center
    verts = []
    for y in [-H/2, H/2]:
        for (cx, cz) in rim:
            verts.append([cx, y, cz])
    verts.append([0, -H/2, 0])   # bottom center (index 2N)
    verts.append([0,  H/2, 0])   # top center    (index 2N+1)
    verts = np.array(verts, dtype=np.float32)

    tris = []
    for i in range(N):
        n = (i+1) % N
        # Side quad
        tris += [i, n, N+n, i, N+n, N+i]
        # Bottom cap
        tris += [2*N, i, n]
        # Top cap
        tris += [2*N+1, N+n, N+i]

    return verts, np.array(tris, dtype=np.uint32)


def make_trade_station():
    """Ring torus approximation: outer ring of box segments + central hub."""
    N   = 12          # segments around the ring
    R   = 8.0         # ring radius
    seg_w, seg_h, seg_d = 2.5, 1.5, 2.5

    parts = []
    for i in range(N):
        angle = 2 * np.pi * i / N
        cx = np.cos(angle) * R
        cz = np.sin(angle) * R
        seg_v, seg_i = box(seg_w, seg_h, seg_d)
        # Rotate each segment to face tangent to ring
        rot = np.array([
            [np.cos(angle), 0, -np.sin(angle)],
            [0,             1,  0            ],
            [np.sin(angle), 0,  np.cos(angle)],
        ])
        seg_v = seg_v @ rot.T + np.array([cx, 0, cz])
        parts.append((seg_v, seg_i))

    # Central hub sphere approximation (icosahedron-like box stack)
    hub_v, hub_i = box(3.0, 3.0, 3.0)
    hub_v = translate(hub_v, 0, 0, 0)
    parts.append((hub_v, hub_i))

    # Spokes connecting hub to ring
    for i in range(0, N, 3):
        angle = 2 * np.pi * i / N
        cx = np.cos(angle) * (R / 2)
        cz = np.sin(angle) * (R / 2)
        sp_v, sp_i = box(0.4, 0.4, R)
        rot = np.array([
            [np.cos(angle), 0, -np.sin(angle)],
            [0,             1,  0            ],
            [np.sin(angle), 0,  np.cos(angle)],
        ])
        sp_v = sp_v @ rot.T + np.array([cx, 0, cz])
        parts.append((sp_v, sp_i))

    return merge(*parts)


def make_moon_surface():
    """Large quad plane at Y=0 representing the lunar surface."""
    SIZE = 200.0
    v = np.array([
        [-SIZE, 0, -SIZE],
        [ SIZE, 0, -SIZE],
        [ SIZE, 0,  SIZE],
        [-SIZE, 0,  SIZE],
    ], dtype=np.float32)
    i = np.array([0,2,1, 0,3,2], dtype=np.uint32)
    return v, i


def make_sphere(radius, rings=12, slices=16):
    """UV sphere of the given radius."""
    verts = []
    tris  = []
    for ring in range(rings + 1):
        phi = np.pi * ring / rings
        for sl in range(slices):
            theta = 2 * np.pi * sl / slices
            verts.append([
                radius * np.sin(phi) * np.cos(theta),
                radius * np.cos(phi),
                radius * np.sin(phi) * np.sin(theta),
            ])
    for ring in range(rings):
        for sl in range(slices):
            ns = (sl + 1) % slices
            a = ring * slices + sl
            b = ring * slices + ns
            c = (ring + 1) * slices + sl
            d = (ring + 1) * slices + ns
            tris += [a, b, d, a, d, c]
    return np.array(verts, dtype=np.float32), np.array(tris, dtype=np.uint32)


def make_star():
    return make_sphere(40.0)

def make_planet_rock():
    return make_sphere(8.0)

def make_planet_earth():
    return make_sphere(15.0)

def make_planet_gas():
    return make_sphere(45.0)

def make_planet_ice():
    return make_sphere(22.0)


def make_tunnel_section():
    """Hollow square tunnel: 4 walls (no end caps) as an 80-unit long passage."""
    LENGTH = 80.0
    HALF   = LENGTH / 2
    W      = 10.0  # inner half-width

    # Each wall: a long thin box
    top_v, top_i   = box(W*2 + 1.5, 1.5, LENGTH)
    top_v = translate(top_v, 0, W, 0)

    bot_v, bot_i   = box(W*2 + 1.5, 1.5, LENGTH)
    bot_v = translate(bot_v, 0, -W, 0)

    left_v, left_i = box(1.5, W*2, LENGTH)
    left_v = translate(left_v, -W, 0, 0)

    right_v, right_i = box(1.5, W*2, LENGTH)
    right_v = translate(right_v,  W, 0, 0)

    return merge(
        (top_v, top_i), (bot_v, bot_i),
        (left_v, left_i), (right_v, right_i),
    )


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

MODELS = [
    ("assets/models/ships/player_ship.glb",         make_player_ship,    (0.3, 0.6, 0.9, 1.0)),
    ("assets/models/ships/pirate_ship.glb",         make_pirate_ship,    (0.7, 0.2, 0.2, 1.0)),
    ("assets/models/props/powerup_neutrino.glb",    make_neutrino_canister, (0.2, 0.9, 1.0, 1.0)),
    ("assets/models/environment/trade_station.glb", make_trade_station,  (0.8, 0.7, 0.3, 1.0)),
    ("assets/models/environment/moon_surface.glb",  make_moon_surface,   (0.6, 0.6, 0.65, 1.0)),
    ("assets/models/environment/tunnel_section_a.glb", make_tunnel_section, (0.4, 0.4, 0.5, 1.0)),
    ("assets/models/environment/star.glb",          make_star,           (1.0, 0.85, 0.10, 1.0)),
    ("assets/models/environment/planet_rock.glb",  make_planet_rock,   (0.55, 0.42, 0.30, 1.0)),
    ("assets/models/environment/planet_earth.glb", make_planet_earth,  (0.20, 0.50, 0.85, 1.0)),
    ("assets/models/environment/planet_gas.glb",   make_planet_gas,    (0.80, 0.62, 0.38, 1.0)),
    ("assets/models/environment/planet_ice.glb",   make_planet_ice,    (0.60, 0.78, 0.92, 1.0)),
]

if __name__ == "__main__":
    print(f"Generating placeholder assets in {REPO_ROOT}/")
    for rel_path, builder, color in MODELS:
        abs_path = os.path.join(REPO_ROOT, rel_path)
        pos, idx = builder()
        gltf = pack_mesh(pos, idx, color)
        save(gltf, abs_path)
    print("Done. Re-run `cmake --build build` to sync assets to the build directory.")
