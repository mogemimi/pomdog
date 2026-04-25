#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$PWD"
BUILD_DIR="$PWD/build"

function generate_app_icons() {
    mkdir -p examples/$1/platform/win32/resources

    $BUILD_DIR/tools/appicon-gen.exe \
        -in assets/icons/pomdog_icon_logo.png \
        -o examples/$1/platform/win32/resources/app.ico \
        --format ico \
        --sizes 16,20,24,32,40,48,64,128,256
}

generate_app_icons "quickstart"
generate_app_icons "pong"
generate_app_icons "feature_showcase"
