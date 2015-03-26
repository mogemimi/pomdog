// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SRC_WIN32_TIMESOURCEWIN32_A31ECBBA_1FF3_46D6_B8EA_5289C0FABFC9_HPP
#define POMDOG_SRC_WIN32_TIMESOURCEWIN32_A31ECBBA_1FF3_46D6_B8EA_5289C0FABFC9_HPP

#include "Pomdog/Application/TimePoint.hpp"

namespace Pomdog {
namespace Detail {
namespace Win32 {

class TimeSourceWin32 {
public:
    TimeSourceWin32();

    TimePoint Now() const;

private:
    double secondsPerTick;
};

}// namespace Win32
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_WIN32_TIMESOURCEWIN32_A31ECBBA_1FF3_46D6_B8EA_5289C0FABFC9_HPP)
