// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/time_point.hpp"

namespace pomdog::detail::apple {

class TimeSourceApple final {
public:
    TimeSourceApple();

    TimePoint Now() const;

private:
    double secondsPerTick;
};

} // namespace pomdog::detail::apple
