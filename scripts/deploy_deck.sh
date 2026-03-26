#!/usr/bin/env bash
# Deploy flightsim to Steam Deck over SSH.
#
# Usage:
#   ./scripts/deploy_deck.sh        # build + sync
#   ./scripts/deploy_deck.sh -r     # build + sync + launch on Deck
#
# Connects to steamdeck.local by default (mDNS hostname).
# Override: DECK_HOST=192.168.x.x ./scripts/deploy_deck.sh

set -euo pipefail
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

DECK_HOST="${DECK_HOST:-steamdeck.local}"
DECK_USER="${DECK_USER:-deck}"
DECK_DIR="/home/$DECK_USER/flightsim"
LAUNCH=0
[[ "${1:-}" == "-r" ]] && LAUNCH=1

# ── Build ─────────────────────────────────────────────────────────────────────
echo "==> Building (Release, Steam Deck)..."
cmake -B "$REPO/build-steamdeck" \
    -DCMAKE_BUILD_TYPE=Release \
    -DSTEAMDECK_BUILD=ON \
    -S "$REPO" \
    --log-level=ERROR
cmake --build "$REPO/build-steamdeck" -- -j"$(nproc)" 2>&1 | \
    grep -E "Linking|error:" || true

# ── Sync ──────────────────────────────────────────────────────────────────────
echo "==> Syncing to $DECK_USER@$DECK_HOST:$DECK_DIR ..."
ssh "$DECK_USER@$DECK_HOST" "mkdir -p $DECK_DIR"

rsync -az --progress \
    "$REPO/build-steamdeck/flightsim" \
    "$DECK_USER@$DECK_HOST:$DECK_DIR/"

rsync -az --delete --progress \
    "$REPO/assets/" \
    "$DECK_USER@$DECK_HOST:$DECK_DIR/assets/"

ssh "$DECK_USER@$DECK_HOST" "
cat > $DECK_DIR/run.sh << 'EOF'
#!/bin/bash
cd \"\$(dirname \"\$0\")\"
exec ./flightsim \"\$@\"
EOF
chmod +x $DECK_DIR/run.sh"

echo "==> Done."

# ── Optional remote launch ────────────────────────────────────────────────────
if [[ $LAUNCH -eq 1 ]]; then
    echo "==> Launching on Deck..."
    ssh -t "$DECK_USER@$DECK_HOST" "cd $DECK_DIR && DISPLAY=:0 ./run.sh"
fi
