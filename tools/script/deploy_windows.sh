#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$PWD"
BUILD_DIR="$PWD/build"

function deploy_windows() {
    mkdir -p $BUILD_DIR/$1/shipping/windows

    cp $BUILD_DIR/windows/examples/$1/Release/$1.exe $BUILD_DIR/$1/shipping/windows/$1.exe
    cp $BUILD_DIR/$1/shipping/desktop/content.idx $BUILD_DIR/$1/shipping/windows/content.idx
    cp $BUILD_DIR/$1/shipping/desktop/content.pak $BUILD_DIR/$1/shipping/windows/content.pak

    find $BUILD_DIR/$1/shipping -name ".DS_Store" -type f -delete
}

deploy_windows "quickstart"
deploy_windows "pong"
deploy_windows "feature_showcase"
