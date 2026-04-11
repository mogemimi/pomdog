#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$PWD"

# NOTE:
# This script intentionally runs `go build` every time before executing the tools.
#
# Normally, the required tools should already be built and located under
# `build/tools` at the repository root. However, this step is necessary to
# handle cross-platform development environments, especially when using WSL2.
#
# In a typical WSL2 setup, developers may have previously run `go build` on
# Windows, which produces Windows executables (`*.exe`) in `build/tools`.
# Although files with the correct names exist, they are not compatible with
# the Linux environment inside WSL2. Attempting to execute them from this
# script will result in errors.
#
# To avoid this issue, we rebuild the tools here to ensure that Linux-compatible
# binaries are generated under `build/tools`, allowing subsequent steps in this
# script to run correctly.
#
# If you are developing exclusively on Linux, this step is technically
# unnecessary, but it is harmless and ensures consistency across environments.
cd $ROOT_DIR/tools/cmd/bundle-deps
go build -o $ROOT_DIR/build/tools/bundle-deps
cd $ROOT_DIR

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
