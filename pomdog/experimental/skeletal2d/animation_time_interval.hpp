// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <chrono>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D {

using AnimationTimeInterval = std::chrono::duration<float, std::ratio<1>>;

static_assert(sizeof(AnimationTimeInterval) == sizeof(float), "");

} // namespace Pomdog::Skeletal2D
