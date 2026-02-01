// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/chrono/time_point.h"
#include "pomdog/chrono/time_source.h"

namespace pomdog::detail::apple {

class TimeSourceApple final : public TimeSource {
private:
    f64 secondsPerTick_ = 0.0;

public:
    TimeSourceApple() noexcept;

    [[nodiscard]] TimePoint
    now() const noexcept override;
};

} // namespace pomdog::detail::apple
