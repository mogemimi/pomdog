// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/chrono/time_point.h"
#include "pomdog/chrono/time_source.h"

namespace pomdog::detail::apple {

class TimeSourceApple final : public TimeSource {
public:
    TimeSourceApple() noexcept;

    [[nodiscard]] TimePoint Now() const noexcept override;

private:
    double secondsPerTick_ = 0.0;
};

} // namespace pomdog::detail::apple
