#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

make -C "$SCRIPT_DIR/src/linux-wine-nativelib/nativelib"

cd "$SCRIPT_DIR/rust-bridge"
make