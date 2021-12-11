// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/chrono/time_point.hpp"
#include "pomdog/chrono/time_source.hpp"

namespace pomdog::detail::win32 {

class TimeSourceWin32 final : public TimeSource {
public:
    TimeSourceWin32() noexcept;

    [[nodiscard]] TimePoint Now() const noexcept override;

private:
    double secondsPerTick_ = 0.0;
};

} // namespace pomdog::detail::win32
