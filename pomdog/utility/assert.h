// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cassert>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

#if !defined(POMDOG_DEBUG_BUILD) || defined(NDEBUG)
#define POMDOG_ASSERT(expression)
#else
#define POMDOG_ASSERT(expression) assert(expression)
#endif

} // namespace pomdog::detail
