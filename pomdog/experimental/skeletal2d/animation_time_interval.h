// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <chrono>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

using AnimationTimeInterval = std::chrono::duration<f32, std::ratio<1>>;

static_assert(sizeof(AnimationTimeInterval) == sizeof(f32), "");

} // namespace pomdog::skeletal2d
