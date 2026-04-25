#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$PWD"

$ROOT_DIR/build/tools/plain-text-format \
    --replace-crlf \
    --remove-space-eol \
    --remove-newline-eof \
    --remove-bom \
    --replace-tabs \
    ".github/workflows/*.yml"

$ROOT_DIR/build/tools/plain-text-format \
    --replace-crlf \
    --remove-space-eol \
    --remove-newline-eof \
    --remove-bom \
    --replace-tabs \
    "schemas/*.fbs" \
    "examples/**/CMakeLists.txt" \
    "cmake/**/CMakeLists.txt" \
    "cmake/pomdog/*.cmake" \
    "test/CMakeLists.txt" \
    "tests/CMakeLists.txt" \
    "CMakeLists.txt"

$ROOT_DIR/build/tools/plain-text-format \
    --replace-crlf \
    --remove-space-eol \
    --remove-newline-eof \
    --remove-bom \
    --replace-tabs \
    "assets/*.toml" \
    "assets/**/*.toml" \
    "assets/shaders/*.slang" \
    "assets/shaders/**/*.slang" \
    "examples/**/assets/*.toml" \
    "examples/**/assets/**/*.toml" \
    "examples/**/assets/shaders/*.slang" \
    "examples/**/assets/shaders/**/*.slang" \
    "examples/**/platform/**/*.html" \
    "examples/**/platform/**/*.js" \
    "examples/**/platform/**/*.rc" \
    "tools/cmd/**/*.toml" \
    "tools/script/*.sh"

$ROOT_DIR/build/tools/plain-text-format \
    --replace-crlf \
    --remove-space-eol \
    --remove-newline-eof \
    "doxygen/Doxyfile" \
    "tools/cmd/**/README.md"

$ROOT_DIR/build/tools/plain-text-format \
    --replace-crlf \
    --remove-newline-eof \
    --remove-bom \
    "tools/cmd/**/go.mod" \
    "tools/cmd/**/go.sum" \
    "tools/cmd/**/*.go" \
    "tools/pkg/**/go.mod" \
    "tools/pkg/**/go.sum" \
    "tools/pkg/**/*.go"

$ROOT_DIR/build/tools/plain-text-format \
    --replace-crlf \
    --remove-newline-eof \
    "docs/*.md" \
    "README.md" \
    "LICENSE.md"
