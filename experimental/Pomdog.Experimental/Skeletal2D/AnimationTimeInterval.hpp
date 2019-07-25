// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <chrono>

namespace Pomdog {

using AnimationTimeInterval = std::chrono::duration<float, std::ratio<1>>;

static_assert(sizeof(AnimationTimeInterval) == sizeof(float), "");

} // namespace Pomdog
