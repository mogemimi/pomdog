// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

POMDOG_CLANG_SUPPRESS_WARNING_PUSH
// FIXME: Suppress the warning about 'consteval' specifier being incompatible with
//        C++ standards before C++20 when compiling with Xcode 16.2.
//        See https://github.com/llvm/llvm-project/issues/138775
//        ```
//        'consteval' specifier is incompatible with C++ standards before C++20 [-Wc++20-compat]
//        ```
POMDOG_CLANG_SUPPRESS_WARNING("-Wc++20-compat")
[[nodiscard]] inline consteval size_t
strlen_compiletime(const char* input) noexcept
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    size_t i = 0;
    while (input[i] != 0) {
        i++;
    }
    return i;
    POMDOG_CLANG_UNSAFE_BUFFER_END
}
POMDOG_CLANG_SUPPRESS_WARNING_POP

} // namespace pomdog::detail
