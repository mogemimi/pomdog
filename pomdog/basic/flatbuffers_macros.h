// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

// NOTE: `-Wdocumentation`: https://github.com/google/flatbuffers/pull/7998
#define POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN \
    POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN             \
                                                                        \
    POMDOG_CLANG_SUPPRESS_WARNING_PUSH                                  \
    POMDOG_CLANG_SUPPRESS_WARNING("-Wdocumentation")                    \
    POMDOG_CLANG_SUPPRESS_WARNING("-Wdocumentation-unknown-command")    \
    POMDOG_CLANG_SUPPRESS_WARNING("-Wextra-semi-stmt")                  \
    POMDOG_CLANG_SUPPRESS_WARNING("-Wnrvo")                             \
    POMDOG_CLANG_SUPPRESS_WARNING("-Wsign-conversion")                  \
    POMDOG_CLANG_SUPPRESS_WARNING("-Wweak-vtables")                     \
                                                                        \
    POMDOG_EMCC_SUPPRESS_WARNING_PUSH                                   \
    POMDOG_EMCC_SUPPRESS_WARNING("-Wunsafe-buffer-usage")               \
    POMDOG_EMCC_SUPPRESS_WARNING("-Wshorten-64-to-32")                  \
                                                                        \
    POMDOG_GCC_SUPPRESS_WARNING_PUSH                                    \
                                                                        \
    POMDOG_MSVC_SUPPRESS_WARNING_PUSH                                   \
    POMDOG_MSVC_SUPPRESS_WARNING(4365)                                  \
    POMDOG_MSVC_SUPPRESS_WARNING(4619)                                  \
    POMDOG_MSVC_SUPPRESS_WARNING(4623)                                  \
    POMDOG_MSVC_SUPPRESS_WARNING(5045)

#define POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END \
    POMDOG_MSVC_SUPPRESS_WARNING_POP                                  \
    POMDOG_GCC_SUPPRESS_WARNING_POP                                   \
    POMDOG_EMCC_SUPPRESS_WARNING_POP                                  \
    POMDOG_CLANG_SUPPRESS_WARNING_POP                                 \
    POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END
