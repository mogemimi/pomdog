// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/TimePoint.hpp"

namespace Pomdog::Detail::Win32 {

class TimeSourceWin32 final {
public:
    TimeSourceWin32();

    TimePoint Now() const;

private:
    double secondsPerTick;
};

} // namespace Pomdog::Detail::Win32
