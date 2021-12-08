// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/time_point.hpp"

namespace pomdog::detail::win32 {

class TimeSourceWin32 final {
public:
    TimeSourceWin32();

    TimePoint Now() const;

private:
    double secondsPerTick;
};

} // namespace pomdog::detail::win32
