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
    "docs/*.md" \
    "README.md" \
    "LICENSE.md"
