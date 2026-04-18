// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/string_compiletime.h"
#include "pomdog/utility/xxhash64.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

inline constexpr u64 string_hash64_seed = 20160723;

} // namespace pomdog::detail

namespace pomdog {

POMDOG_CLANG_SUPPRESS_WARNING_PUSH
// FIXME: Suppress the warning about 'consteval' specifier being incompatible with
//        C++ standards before C++20 when compiling with Xcode 16.2.
//        See https://github.com/llvm/llvm-project/issues/138775
//        ```
//        'consteval' specifier is incompatible with C++ standards before C++20 [-Wc++20-compat]
//        ```
POMDOG_CLANG_SUPPRESS_WARNING("-Wc++20-compat")
[[nodiscard]] inline consteval u64
computeStringHash64(const char* s) noexcept
{
    return pomdog::hash::xxh64(s, detail::strlen_compiletime(s), detail::string_hash64_seed);
}
POMDOG_CLANG_SUPPRESS_WARNING_POP

[[nodiscard]] u64
computeStringHash64(std::string_view s) noexcept;

} // namespace pomdog
