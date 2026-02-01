// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/chrono/time_point.h"
#include "pomdog/chrono/time_source.h"

namespace pomdog::detail::win32 {

class TimeSourceWin32 final : public TimeSource {
public:
    TimeSourceWin32() noexcept;

    [[nodiscard]] TimePoint
    now() const noexcept override;

private:
    f64 secondsPerTick_ = 0.0;
};

} // namespace pomdog::detail::win32
