#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$PWD"
TOOLS_DIR="$ROOT_DIR/build/tools"

function build_app() {
    local APP_NAME="$1"
    local BUILD_DIR="$ROOT_DIR/build/$APP_NAME"
    local CONTENT_DIR="$BUILD_DIR/content"
    local APP_DIR="$ROOT_DIR/examples/$APP_NAME"

    mkdir -p $BUILD_DIR/shaderbuild
    mkdir -p $BUILD_DIR/copybuild
    mkdir -p $BUILD_DIR/convertbuild
    mkdir -p $BUILD_DIR/archivebuild
    mkdir -p $BUILD_DIR/archive/build

    # 1. Generate shader ninja build (engine shaders)
    $TOOLS_DIR/shader-ninja-gen \
        -recipe $ROOT_DIR/assets/shaders/shaderbuild.toml \
        -indir $ROOT_DIR/assets/shaders \
        -outninja $BUILD_DIR/shaderbuild/shaders_pomdog.ninja \
        -outdir $CONTENT_DIR/shaders \
        -intdir $BUILD_DIR/shaderbuild \
        -tooldir $TOOLS_DIR \
        -link-validate \
        -outarchive $BUILD_DIR/archive/build/archive_shaders_pomdog.toml

    # 2. Generate shader ninja build (app shaders)
    $TOOLS_DIR/shader-ninja-gen \
        -recipe $APP_DIR/assets/shaders/shaderbuild.toml \
        -indir $APP_DIR/assets/shaders \
        -outninja $BUILD_DIR/shaderbuild/shaders_app.ninja \
        -outdir $CONTENT_DIR/shaders \
        -intdir $BUILD_DIR/shaderbuild \
        -tooldir $TOOLS_DIR \
        -link-validate \
        -outarchive $BUILD_DIR/archive/build/archive_shaders_$APP_NAME.toml

    # 3. Generate copy ninja build (engine fonts)
    $TOOLS_DIR/copy-ninja-gen \
        -recipe $ROOT_DIR/assets/copybuild.toml \
        -indir $ROOT_DIR/assets \
        -outninja $BUILD_DIR/copybuild/copy_pomdog.ninja \
        -outdir $CONTENT_DIR

    # 4. Generate copy ninja build (app assets)
    $TOOLS_DIR/copy-ninja-gen \
        -recipe $APP_DIR/assets/copybuild.toml \
        -indir $APP_DIR/assets \
        -outninja $BUILD_DIR/copybuild/copy_app.ninja \
        -outdir $CONTENT_DIR

    # 5. Generate asset-convert ninja build (engine assets)
    $TOOLS_DIR/asset-convert-ninja-gen \
        -recipe $ROOT_DIR/assets/assetconvert.toml \
        -thirdpartydir $ROOT_DIR/thirdparty \
        -outninja $BUILD_DIR/convertbuild/convert_pomdog.ninja \
        -outdir $CONTENT_DIR \
        -tooldir $TOOLS_DIR

    # 6. Generate archive ninja build
    $TOOLS_DIR/archive-ninja-gen \
        -outninja $BUILD_DIR/archivebuild/build.ninja \
        -contentdir $CONTENT_DIR \
        -outdir $BUILD_DIR/shipping \
        -intdir $BUILD_DIR/archivebuild \
        -tooldir $TOOLS_DIR \
        -dep-subninja $BUILD_DIR/shaderbuild/shaders_pomdog.ninja \
        -dep-subninja $BUILD_DIR/shaderbuild/shaders_app.ninja \
        -dep-subninja $BUILD_DIR/copybuild/copy_pomdog.ninja \
        -dep-subninja $BUILD_DIR/copybuild/copy_app.ninja \
        -dep-subninja $BUILD_DIR/convertbuild/convert_pomdog.ninja \
        $ROOT_DIR/assets/archive/archive_fonts.toml \
        $ROOT_DIR/assets/archive/archive_game_controller_db.toml \
        $BUILD_DIR/archive/build/archive_shaders_pomdog.toml \
        $BUILD_DIR/archive/build/archive_shaders_$APP_NAME.toml \
        $APP_DIR/assets/archive/archive.toml

    # 7. Generate top-level ninja build combining all sub-builds
    $TOOLS_DIR/subninja-gen \
        -o $BUILD_DIR/build.ninja \
        $BUILD_DIR/shaderbuild/shaders_pomdog.ninja \
        $BUILD_DIR/shaderbuild/shaders_app.ninja \
        $BUILD_DIR/copybuild/copy_pomdog.ninja \
        $BUILD_DIR/copybuild/copy_app.ninja \
        $BUILD_DIR/convertbuild/convert_pomdog.ninja \
        $BUILD_DIR/archivebuild/build.ninja

    # 8. Run all builds with a single ninja invocation
    $TOOLS_DIR/ninja -C $BUILD_DIR
}

build_app "quickstart"
build_app "pong"
build_app "feature_showcase"
