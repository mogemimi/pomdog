// Copyright mogemimi. Distributed under the MIT license.

package main

import "fmt"

func generateBootstrapScript() string {
	return `#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$PWD"
POMDOG_DIR="$ROOT_DIR/pomdog"

cd $POMDOG_DIR/tools/cmd/bootstrap-toolchain
go build -o $ROOT_DIR/build/tools/bootstrap-toolchain
cd $ROOT_DIR

BOOTSTRAP_ARGS=(
    --pomdogdir $POMDOG_DIR
    --builddir $ROOT_DIR/build
    --appdir $ROOT_DIR
    $POMDOG_DIR/tools/cmd/bootstrap-toolchain/bootstrap.toml
)

if [ -f "$ROOT_DIR/tools/bootstrap.toml" ]; then
    BOOTSTRAP_ARGS+=("$ROOT_DIR/tools/bootstrap.toml")
fi

./build/tools/bootstrap-toolchain "${BOOTSTRAP_ARGS[@]}"
`
}

func generateBuildToolsScript() string {
	return `#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$PWD"
POMDOG_DIR="$ROOT_DIR/pomdog"

cd $POMDOG_DIR/tools/cmd/bootstrap-toolchain
go build -o $ROOT_DIR/build/tools/bootstrap-toolchain
cd $ROOT_DIR

BOOTSTRAP_ARGS=(
    --pomdogdir $POMDOG_DIR
    --builddir $ROOT_DIR/build
    --appdir $ROOT_DIR
    --skip-cpp
    --skip-download
    $POMDOG_DIR/tools/cmd/bootstrap-toolchain/bootstrap.toml
)

if [ -f "$ROOT_DIR/tools/bootstrap.toml" ]; then
    BOOTSTRAP_ARGS+=("$ROOT_DIR/tools/bootstrap.toml")
fi

./build/tools/bootstrap-toolchain "${BOOTSTRAP_ARGS[@]}"
`
}

func generateAssetBuildScript(name string) string {
	return fmt.Sprintf(`#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$PWD"
POMDOG_DIR="$ROOT_DIR/pomdog"
TOOLS_DIR="$ROOT_DIR/build/tools"
APP_NAME="%s"
BUILD_DIR="$ROOT_DIR/build/$APP_NAME"
CONTENT_DIR="$BUILD_DIR/content"

mkdir -p $BUILD_DIR/shaderbuild
mkdir -p $BUILD_DIR/copybuild
mkdir -p $BUILD_DIR/convertbuild
mkdir -p $BUILD_DIR/archivebuild
mkdir -p $BUILD_DIR/archive/build

# 1. Generate shader ninja build (engine shaders)
$TOOLS_DIR/shader-ninja-gen \
    -recipe $POMDOG_DIR/assets/shaders/shaderbuild.toml \
    -indir $POMDOG_DIR/assets/shaders \
    -outninja $BUILD_DIR/shaderbuild/shaders_pomdog.ninja \
    -outdir $CONTENT_DIR/shaders \
    -intdir $BUILD_DIR/shaderbuild \
    -tooldir $TOOLS_DIR \
    -link-validate \
    -outarchive $BUILD_DIR/archive/build/archive_shaders_pomdog.toml

# 2. Generate shader ninja build (app shaders)
$TOOLS_DIR/shader-ninja-gen \
    -recipe $ROOT_DIR/assets/shaders/shaderbuild.toml \
    -indir $ROOT_DIR/assets/shaders \
    -outninja $BUILD_DIR/shaderbuild/shaders_app.ninja \
    -outdir $CONTENT_DIR/shaders \
    -intdir $BUILD_DIR/shaderbuild \
    -tooldir $TOOLS_DIR \
    -link-validate \
    -outarchive $BUILD_DIR/archive/build/archive_shaders_app.toml

# 3. Generate copy ninja build (engine assets)
$TOOLS_DIR/copy-ninja-gen \
    -recipe $POMDOG_DIR/assets/copybuild.toml \
    -indir $POMDOG_DIR/assets \
    -outninja $BUILD_DIR/copybuild/copy_pomdog.ninja \
    -outdir $CONTENT_DIR

# 4. Generate copy ninja build (app assets)
$TOOLS_DIR/copy-ninja-gen \
    -recipe $ROOT_DIR/assets/copybuild.toml \
    -indir $ROOT_DIR/assets \
    -outninja $BUILD_DIR/copybuild/copy_app.ninja \
    -outdir $CONTENT_DIR

# 5. Generate asset-convert ninja build (engine assets)
$TOOLS_DIR/asset-convert-ninja-gen \
    -recipe $POMDOG_DIR/assets/assetconvert.toml \
    -indir $POMDOG_DIR/assets \
    -thirdpartydir $POMDOG_DIR/thirdparty \
    -intdir $BUILD_DIR/convertbuild \
    -outninja $BUILD_DIR/convertbuild/convert_pomdog.ninja \
    -outdir $CONTENT_DIR \
    -tooldir $TOOLS_DIR

# 6. Generate asset-convert ninja build (app assets)
$TOOLS_DIR/asset-convert-ninja-gen \
    -recipe $ROOT_DIR/assets/assetconvert.toml \
    -indir $ROOT_DIR/assets \
    -thirdpartydir $POMDOG_DIR/thirdparty \
    -intdir $BUILD_DIR/convertbuild \
    -outninja $BUILD_DIR/convertbuild/convert_app.ninja \
    -outdir $CONTENT_DIR \
    -tooldir $TOOLS_DIR

# 7. Generate archive ninja build
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
    -dep-subninja $BUILD_DIR/convertbuild/convert_app.ninja \
    $POMDOG_DIR/assets/archive/archive_fonts.toml \
    $POMDOG_DIR/assets/archive/archive_game_controller_db.toml \
    $BUILD_DIR/archive/build/archive_shaders_pomdog.toml \
    $BUILD_DIR/archive/build/archive_shaders_app.toml \
    $ROOT_DIR/assets/archive/archive.toml

# 8. Generate combined ninja build
$TOOLS_DIR/subninja-gen \
    -o $BUILD_DIR/build.ninja \
    $BUILD_DIR/shaderbuild/shaders_pomdog.ninja \
    $BUILD_DIR/shaderbuild/shaders_app.ninja \
    $BUILD_DIR/copybuild/copy_pomdog.ninja \
    $BUILD_DIR/copybuild/copy_app.ninja \
    $BUILD_DIR/convertbuild/convert_pomdog.ninja \
    $BUILD_DIR/convertbuild/convert_app.ninja \
    $BUILD_DIR/archivebuild/build.ninja

# 9. Run all builds with a single ninja invocation
$TOOLS_DIR/ninja -C $BUILD_DIR
`, name)
}

func generatePackageWindowsScript(name string) string {
	return fmt.Sprintf(`#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="$PWD/build"
APP_NAME="%s"

mkdir -p $BUILD_DIR/$APP_NAME/shipping/windows

cp $BUILD_DIR/windows/Release/$APP_NAME.exe $BUILD_DIR/$APP_NAME/shipping/windows/$APP_NAME.exe
cp $BUILD_DIR/$APP_NAME/shipping/desktop/content.idx $BUILD_DIR/$APP_NAME/shipping/windows/content.idx
cp $BUILD_DIR/$APP_NAME/shipping/desktop/content.pak $BUILD_DIR/$APP_NAME/shipping/windows/content.pak

find $BUILD_DIR/$APP_NAME/shipping -name ".DS_Store" -type f -delete
`, name)
}

func generatePackageLinuxScript(name string) string {
	return fmt.Sprintf(`#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$PWD"
BUILD_DIR="$PWD/build"
APP_NAME="%s"

mkdir -p $BUILD_DIR/$APP_NAME/shipping/linux

cp $BUILD_DIR/linux_release/$APP_NAME $BUILD_DIR/$APP_NAME/shipping/linux/$APP_NAME
cp $BUILD_DIR/$APP_NAME/shipping/desktop/content.idx $BUILD_DIR/$APP_NAME/shipping/linux/content.idx
cp $BUILD_DIR/$APP_NAME/shipping/desktop/content.pak $BUILD_DIR/$APP_NAME/shipping/linux/content.pak

# Remove old bundled dependencies if they exist
if [ -d "$BUILD_DIR/$APP_NAME/shipping/linux/lib" ]; then
    find $BUILD_DIR/$APP_NAME/shipping/linux/lib -name "*.so" -type f -delete
    find $BUILD_DIR/$APP_NAME/shipping/linux/lib -name "*.so.*" -type f -delete
fi

# Bundle shared library dependencies
$ROOT_DIR/build/tools/bundle-deps -v \
    -o $BUILD_DIR/$APP_NAME/shipping/linux/lib \
    $BUILD_DIR/$APP_NAME/shipping/linux/$APP_NAME

find $BUILD_DIR/$APP_NAME/shipping -name ".DS_Store" -type f -delete
`, name)
}

func generatePackageEmscriptenScript(name string) string {
	return fmt.Sprintf(`#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$PWD"
BUILD_DIR="$PWD/build"
APP_NAME="%s"

mkdir -p $BUILD_DIR/$APP_NAME/shipping/web

cp $BUILD_DIR/emscripten_release/$APP_NAME.js $BUILD_DIR/$APP_NAME/shipping/web/$APP_NAME.js
cp $BUILD_DIR/emscripten_release/$APP_NAME.wasm $BUILD_DIR/$APP_NAME/shipping/web/$APP_NAME.wasm
cp $ROOT_DIR/platform/emscripten/index.html $BUILD_DIR/$APP_NAME/shipping/web/index.html

find $BUILD_DIR/$APP_NAME/shipping -name ".DS_Store" -type f -delete
`, name)
}

func generateClangFormatScript() string {
	return `#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$PWD"

$ROOT_DIR/build/tools/clang-format-all \
    "source/*.h" \
    "source/*.cpp" \
    "platform/**/*.cpp" \
    "platform/**/*.h" \
    "platform/**/*.mm"
`
}

func generateDocsFormatScript() string {
	return `#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$PWD"

$ROOT_DIR/build/tools/plain-text-format \
    --replace-crlf \
    --remove-space-eol \
    --remove-newline-eof \
    --remove-bom \
    --replace-tabs \
    "CMakeLists.txt"
`
}

func generateBootstrapToml() string {
	return `# App-specific bootstrap configuration.
# Add your own Go tools, download tools, or schemas here.
# See pomdog/tools/cmd/bootstrap-toolchain/bootstrap.toml for examples.
#
# Example: Adding a custom Go tool
#   [[go_tools]]
#   go_dir = "${APP_DIR}/tools/cmd"
#   tools = ["my-tool"]
`
}

func generateGitignore() string {
	return `*.a
*.exe
*.filters
*.lib
*.pdb
*.props
*.sdf
*.sln
*.so
*.suo
*.user
*.vcproj
*.vcxproj
.DS_Store
/build/

# Xcode
*.pbxuser
!default.pbxuser
*.mode1v3
!default.mode1v3
*.mode2v3
!default.mode2v3
*.perspectivev3
!default.perspectivev3
xcuserdata
*.xccheckout
*.moved-aside
`
}
