// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/time_point.hpp"

namespace Pomdog::Detail::Apple {

class TimeSourceApple final {
public:
    TimeSourceApple();

    TimePoint Now() const;

private:
    double secondsPerTick;
};

} // namespace Pomdog::Detail::Apple
