// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cassert>
#include <source_location>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

// NOTE: Define `POMDOG_USE_CUSTOM_ASSERT` to route `POMDOG_ASSERT` failures to
// `reportAssertion()` instead of the standard assert macro.

#if defined(POMDOG_USE_CUSTOM_ASSERT)

#if defined(__clang__) || defined(__GNUC__)
#define POMDOG_DETAIL_LIKELY_EXPR(expr) __builtin_expect(!!(expr), 1)
#else
#define POMDOG_DETAIL_LIKELY_EXPR(expr) (!!(expr))
#endif

/// Reports a failed `POMDOG_ASSERT` expression.
///
/// `POMDOG_ASSERT` calls this function when both `POMDOG_DEBUG_BUILD` and
/// `POMDOG_USE_CUSTOM_ASSERT` are defined. Applications or platform layers that
/// enable custom asserts must provide an implementation.
///
/// The `message` is the stringized assertion expression. The `location` identifies
/// the source file, line, column, and function where `POMDOG_ASSERT` was expanded.
///
/// This function does not return. Typical implementations log the assertion
/// failure, trigger a debugger breakpoint, show a diagnostic dialog, or terminate
/// the program.
[[noreturn]] void
reportAssertion(
    const char* message,
    std::source_location location) noexcept;

#endif

} // namespace pomdog::detail

// NOTE: `POMDOG_ASSERT` checks expression only in debug builds. When
// `POMDOG_USE_CUSTOM_ASSERT` is also defined, assertion failures are reported
// through `pomdog::detail::reportAssertion()`; otherwise, the standard assert macro
// is used.
#if defined(POMDOG_DEBUG_BUILD) && defined(POMDOG_USE_CUSTOM_ASSERT)
#define POMDOG_ASSERT(expression)                     \
    do {                                              \
        if (!POMDOG_DETAIL_LIKELY_EXPR(expression)) { \
            pomdog::detail::reportAssertion(          \
                #expression,                          \
                std::source_location::current());     \
        }                                             \
    } while (false)
#elif defined(POMDOG_DEBUG_BUILD)
#define POMDOG_ASSERT(expression) assert(expression)
#else
#define POMDOG_ASSERT(expression) static_cast<void>(0)
#endif
