// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cassert>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

#if defined(NDEBUG)
#define POMDOG_ASSERT(expression)
#define POMDOG_ASSERT_MESSAGE(expression, message)
#else
#define POMDOG_ASSERT(expression) assert(expression)
#define POMDOG_ASSERT_MESSAGE(expression, message) assert(expression)
#endif

} // namespace pomdog::detail
