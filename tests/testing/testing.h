// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

#define POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN  \
    POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN          \
    POMDOG_CLANG_SUPPRESS_WARNING_PUSH                               \
    POMDOG_CLANG_SUPPRESS_WARNING("-Wdouble-promotion")              \
    POMDOG_CLANG_SUPPRESS_WARNING("-Wunsafe-buffer-usage")           \
    POMDOG_CLANG_SUPPRESS_WARNING("-Wimplicit-int-float-conversion") \
                                                                     \
    POMDOG_EMCC_SUPPRESS_WARNING_PUSH                                \
    POMDOG_EMCC_SUPPRESS_WARNING("-Wunsafe-buffer-usage")            \
                                                                     \
    POMDOG_GCC_SUPPRESS_WARNING_PUSH                                 \
                                                                     \
    POMDOG_MSVC_SUPPRESS_WARNING_PUSH                                \
    POMDOG_MSVC_SUPPRESS_WARNING(4866)

#define POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END \
    POMDOG_MSVC_SUPPRESS_WARNING_POP                              \
    POMDOG_GCC_SUPPRESS_WARNING_POP                               \
    POMDOG_EMCC_SUPPRESS_WARNING_POP                              \
    POMDOG_CLANG_SUPPRESS_WARNING_POP                             \
    POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

#if defined(__EMSCRIPTEN__)
#define POMDOG_TESTING_ENABLE_THROW_EXCEPTIONS 0
#else
#define POMDOG_TESTING_ENABLE_THROW_EXCEPTIONS 1
#endif
