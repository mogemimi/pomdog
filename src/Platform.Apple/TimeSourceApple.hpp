// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_TIMESOURCEAPPLE_3820AACF_HPP
#define POMDOG_TIMESOURCEAPPLE_3820AACF_HPP

#include "Pomdog/Application/TimePoint.hpp"

namespace Pomdog {
namespace Detail {
namespace Apple {

class TimeSourceApple final {
public:
    TimeSourceApple();

    TimePoint Now() const;

private:
    double secondsPerTick;
};

} // namespace Apple
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_TIMESOURCEAPPLE_3820AACF_HPP
