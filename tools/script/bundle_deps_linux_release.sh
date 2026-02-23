#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$PWD"

# Build the bundle-deps tool
cd "$ROOT_DIR/tools/cmd/bundle-deps"
go build -o "$ROOT_DIR/build/tools/bundle-deps"
cd "$ROOT_DIR"

# Bundle dependencies for examples and tests
"$ROOT_DIR/build/tools/bundle-deps" -v \
    -o "$ROOT_DIR/build/linux_release/examples/feature_showcase/lib" \
    "$ROOT_DIR/build/linux_release/examples/feature_showcase/feature_showcase"

"$ROOT_DIR/build/tools/bundle-deps" -v \
    -o "$ROOT_DIR/build/linux_release/examples/pong/lib" \
    "$ROOT_DIR/build/linux_release/examples/pong/pong"

"$ROOT_DIR/build/tools/bundle-deps" -v \
    -o "$ROOT_DIR/build/linux_release/examples/quickstart/lib" \
    "$ROOT_DIR/build/linux_release/examples/quickstart/quickstart"

"$ROOT_DIR/build/tools/bundle-deps" -v \
    -o "$ROOT_DIR/build/linux_release/tests/lib" \
    "$ROOT_DIR/build/linux_release/tests/pomdog_tests"
