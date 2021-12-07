// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/time_point.hpp"

namespace Pomdog::Detail::Win32 {

class TimeSourceWin32 final {
public:
    TimeSourceWin32();

    TimePoint Now() const;

private:
    double secondsPerTick;
};

} // namespace Pomdog::Detail::Win32
