// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN

// FIXME: Avoid compile errors on Visual Studio 2019
#if defined(_MSC_VER) && (_MSC_VER > 1900)
#define FMT_USE_NONTYPE_TEMPLATE_ARGS 0
#endif

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4061)
POMDOG_MSVC_SUPPRESS_WARNING(4127)
POMDOG_MSVC_SUPPRESS_WARNING(4365)
POMDOG_MSVC_SUPPRESS_WARNING(4582)
POMDOG_MSVC_SUPPRESS_WARNING(4623)
POMDOG_MSVC_SUPPRESS_WARNING(4996)
POMDOG_CLANG_SUPPRESS_WARNING_PUSH
POMDOG_CLANG_SUPPRESS_WARNING("-Wdisabled-macro-expansion")
POMDOG_CLANG_SUPPRESS_WARNING("-Wdocumentation-unknown-command")
POMDOG_CLANG_SUPPRESS_WARNING("-Wduplicate-enum")
POMDOG_CLANG_SUPPRESS_WARNING("-Wswitch-enum")
POMDOG_CLANG_SUPPRESS_WARNING("-Wswitch-default")
POMDOG_CLANG_SUPPRESS_WARNING("-Wundefined-func-template")
POMDOG_CLANG_SUPPRESS_WARNING("-Wunsafe-buffer-usage")
POMDOG_CLANG_SUPPRESS_WARNING("-Wunsafe-buffer-usage-in-libc-call")

// FIXME: Suppress the warning about 'consteval' specifier being incompatible with
//        C++ standards before C++20 when compiling with Xcode 16.2.
//        See https://github.com/llvm/llvm-project/issues/138775
//        ```
//        'consteval' specifier is incompatible with C++ standards before C++20 [-Wc++20-compat]
//        ```
POMDOG_CLANG_SUPPRESS_WARNING("-Wc++20-compat")

POMDOG_EMCC_SUPPRESS_WARNING_PUSH
POMDOG_EMCC_SUPPRESS_WARNING("-Wlifetime-safety-intra-tu-suggestions")
#include <fmt/args.h>
#include <fmt/format.h>
POMDOG_EMCC_SUPPRESS_WARNING_POP
POMDOG_CLANG_SUPPRESS_WARNING_POP
POMDOG_MSVC_SUPPRESS_WARNING_POP

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Formats a string using the fmt library.
/// The format string is checked at compile time, so if the format string is invalid, a compile error will occur.
template <typename... Args>
[[nodiscard]] inline auto format(fmt::format_string<Args...>&& fmt, Args&&... args) -> std::string
{
    return fmt::format(std::forward<fmt::format_string<Args...>>(fmt), std::forward<Args>(args)...);
}

using dynamic_format_arg_store = fmt::dynamic_format_arg_store<fmt::format_context>;

/// Formats a string using the fmt library with a dynamic format string and arguments.
template <typename T>
[[nodiscard]] inline auto vformat(T&& fmt, dynamic_format_arg_store&& store) -> std::string
{
    try {
        return fmt::vformat(std::forward<T>(fmt), std::forward<dynamic_format_arg_store>(store));
    }
    catch (...) {
    }
    return {};
}

} // namespace pomdog
