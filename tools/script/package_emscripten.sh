#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$PWD"
BUILD_DIR="$PWD/build"

function package_emscripten() {
    mkdir -p $BUILD_DIR/$1/shipping/web

    cp $BUILD_DIR/emscripten_release/examples/$1/$1.js $BUILD_DIR/$1/shipping/web/$1.js
    cp $BUILD_DIR/emscripten_release/examples/$1/$1.wasm $BUILD_DIR/$1/shipping/web/$1.wasm
    cp $ROOT_DIR/examples/$1/platform/emscripten/index.html $BUILD_DIR/$1/shipping/web/index.html

    find $BUILD_DIR/$1/shipping -name ".DS_Store" -type f -delete
}

package_emscripten "quickstart"
package_emscripten "pong"
package_emscripten "feature_showcase"
