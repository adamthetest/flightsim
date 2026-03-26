#!/usr/bin/env bash
# Build and package FlightSim for Steam Deck distribution.
# Produces: flightsim-steamdeck.tar.gz in the repo root.
#
# Requirements: cmake, a C++17 cross-compiler (or build natively on Deck),
#               and the repo cloned at ~/code/flightsim.
#
# Run on the Steam Deck itself or via SteamOS SDK container.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${REPO_ROOT}/build-steamdeck"
DIST_DIR="${REPO_ROOT}/dist/flightsim"

echo "==> Configuring (Steam Deck / znver2)..."
cmake -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DSTEAMDECK_BUILD=ON \
    -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++" \
    "${REPO_ROOT}"

echo "==> Building..."
cmake --build "${BUILD_DIR}" --config Release -- -j"$(nproc)"

echo "==> Packaging..."
rm -rf "${DIST_DIR}"
mkdir -p "${DIST_DIR}"

cp "${BUILD_DIR}/flightsim" "${DIST_DIR}/"
cp -r "${REPO_ROOT}/assets"  "${DIST_DIR}/"

# Steam Deck launch script
cat > "${DIST_DIR}/launch.sh" << 'LAUNCH'
#!/bin/bash
cd "$(dirname "$0")"
exec ./flightsim "$@"
LAUNCH
chmod +x "${DIST_DIR}/launch.sh"

ARCHIVE="${REPO_ROOT}/flightsim-steamdeck.tar.gz"
tar -czf "${ARCHIVE}" -C "${REPO_ROOT}/dist" flightsim

echo "==> Done: ${ARCHIVE}"
echo "    Copy to Steam Deck and add launch.sh as a Non-Steam Game."
