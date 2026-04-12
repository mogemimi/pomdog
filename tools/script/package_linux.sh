#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$PWD"
BUILD_DIR="$PWD/build"

# # Build the bundle-deps tool
# cd $ROOT_DIR/tools/cmd/bundle-deps
# go build -o $ROOT_DIR/build/tools/bundle-deps
# cd $ROOT_DIR

function package_linux() {
    mkdir -p $BUILD_DIR/$1/shipping/linux

    cp $BUILD_DIR/linux_release/examples/$1/$1 $BUILD_DIR/$1/shipping/linux/$1
    cp $BUILD_DIR/$1/shipping/desktop/content.idx $BUILD_DIR/$1/shipping/linux/content.idx
    cp $BUILD_DIR/$1/shipping/desktop/content.pak $BUILD_DIR/$1/shipping/linux/content.pak

    # Remove the old bundled dependencies if they exist
    if [ -d "$BUILD_DIR/$1/shipping/linux/lib" ]; then
        find $BUILD_DIR/$1/shipping/linux/lib -name "*.so" -type f -delete
        find $BUILD_DIR/$1/shipping/linux/lib -name "*.so.*" -type f -delete
    fi

    # Bundle shared library dependencies
    $ROOT_DIR/build/tools/bundle-deps -v \
        -o $BUILD_DIR/$1/shipping/linux/lib \
        $BUILD_DIR/$1/shipping/linux/$1

    find $BUILD_DIR/$1/shipping -name ".DS_Store" -type f -delete
}

package_linux "quickstart"
package_linux "pong"
package_linux "feature_showcase"
