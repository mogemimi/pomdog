//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_ANIMATIONTIMEINTERVAL_DEA1AD60_E2E2_45F6_9B79_A2DDB30EDC4E_HPP
#define POMDOG_ANIMATIONTIMEINTERVAL_DEA1AD60_E2E2_45F6_9B79_A2DDB30EDC4E_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <chrono>

namespace Pomdog {

using AnimationTimeInterval = std::chrono::duration<float, std::ratio<1>>;

static_assert(sizeof(AnimationTimeInterval) == sizeof(float), "");

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONTIMEINTERVAL_DEA1AD60_E2E2_45F6_9B79_A2DDB30EDC4E_HPP)
