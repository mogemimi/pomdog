ROOT_DIR="$PWD"

$ROOT_DIR/build/tools/clang-format-all \
    "pomdog/*/*.h" \
    "pomdog/*/*/*.h" \
    "pomdog/*/*.cpp" \
    "pomdog/*/*/*.cpp" \
    "pomdog/*/*.mm" \
    "pomdog/*/*/*.mm" \
    "test/*.cpp" \
    "test/*/*.cpp" \
    "test/*/*.h"

# $ROOT_DIR/build/tools/clang-format-all \
#     "examples/**/**/**/*.cpp" \
#     "examples/**/**/**/*.h" \
#     "examples/**/**/**/*.mm" \
#     "examples/**/**/*.cpp" \
#     "examples/**/**/*.h" \
#     "examples/**/**/*.mm"

$ROOT_DIR/build/tools/plain-text-format \
    --replace-crlf \
    --remove-space-eol \
    --remove-newline-eof \
    "examples/**/**/**/*.cpp" \
    "examples/**/**/**/*.h" \
    "examples/**/**/**/*.mm" \
    "examples/**/**/*.cpp" \
    "examples/**/**/*.h" \
    "examples/**/**/*.mm" \
    "pomdog/*/*.h" \
    "pomdog/*/*/*.h" \
    "pomdog/*/*.cpp" \
    "pomdog/*/*/*.cpp" \
    "pomdog/*/*.mm" \
    "pomdog/*/*/*.mm" \
    "test/*.cpp" \
    "test/*/*.cpp" \
    "test/*/*.h"
