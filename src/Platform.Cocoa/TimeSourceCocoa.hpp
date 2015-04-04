// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TIMESOURCECOCOA_3820AACF_HPP
#define POMDOG_TIMESOURCECOCOA_3820AACF_HPP

#include "Pomdog/Application/TimePoint.hpp"

namespace Pomdog {
namespace Detail {
namespace Cocoa {

class TimeSourceCocoa final {
public:
    TimeSourceCocoa();

    TimePoint Now() const;

private:
    double secondsPerTick;
};

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_TIMESOURCECOCOA_3820AACF_HPP
