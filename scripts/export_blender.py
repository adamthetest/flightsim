#!/usr/bin/env python3
"""
Headless Blender batch exporter.
Usage: blender --background --python scripts/export_blender.py

Exports all objects (or collections) in the .blend file to GLTF 2.0 (.glb).
Run from the repo root. Expects BLEND_FILE and OUTPUT_DIR env vars, or
edit the defaults below.

Convention:
  - 1 Blender unit = 1 meter
  - Y-up (matches Raylib, no flip needed)
  - Apply modifiers ON, PBR metallic-roughness, textures embedded
"""
import bpy
import os
import sys

BLEND_FILE = os.environ.get("BLEND_FILE", "")   # set externally if needed
OUTPUT_DIR = os.environ.get("OUTPUT_DIR", os.path.join(os.path.dirname(__file__),
                                                        "..", "assets", "models"))


def export_collection(collection, out_dir: str):
    os.makedirs(out_dir, exist_ok=True)
    for obj in collection.objects:
        if obj.type != "MESH":
            continue
        out_path = os.path.join(out_dir, f"{obj.name}.glb")
        # Select only this object
        bpy.ops.object.select_all(action="DESELECT")
        obj.select_set(True)
        bpy.context.view_layer.objects.active = obj
        bpy.ops.export_scene.gltf(
            filepath=out_path,
            export_format="GLB",
            use_selection=True,
            export_apply=True,           # apply modifiers
            export_materials="EXPORT",
            export_image_format="AUTO",
            export_cameras=False,
            export_lights=False,
            export_animations=True,
            export_yup=True,             # Y-up matches Raylib
        )
        print(f"Exported: {out_path}")


def main():
    if BLEND_FILE and os.path.exists(BLEND_FILE):
        bpy.ops.wm.open_mainfile(filepath=BLEND_FILE)

    # Map collection names → output subdirectories
    collection_map = {
        "Ships":       os.path.join(OUTPUT_DIR, "ships"),
        "Environment": os.path.join(OUTPUT_DIR, "environment"),
        "Props":       os.path.join(OUTPUT_DIR, "props"),
    }

    for col_name, out_dir in collection_map.items():
        col = bpy.data.collections.get(col_name)
        if col:
            export_collection(col, out_dir)
            print(f"Collection '{col_name}' → {out_dir}")
        else:
            print(f"Warning: collection '{col_name}' not found, skipping.")

    print("Export complete.")


if __name__ == "__main__":
    main()
