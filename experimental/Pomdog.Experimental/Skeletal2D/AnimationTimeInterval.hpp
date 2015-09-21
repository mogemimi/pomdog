// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ANIMATIONTIMEINTERVAL_DEA1AD60_HPP
#define POMDOG_ANIMATIONTIMEINTERVAL_DEA1AD60_HPP

#include <chrono>

namespace Pomdog {

using AnimationTimeInterval = std::chrono::duration<float, std::ratio<1>>;

static_assert(sizeof(AnimationTimeInterval) == sizeof(float), "");

} // namespace Pomdog

#endif // POMDOG_ANIMATIONTIMEINTERVAL_DEA1AD60_HPP
