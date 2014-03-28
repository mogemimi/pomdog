//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_WIN32_TIMESOURCEWIN32_A31ECBBA_1FF3_46D6_B8EA_5289C0FABFC9_HPP
#define POMDOG_SRC_WIN32_TIMESOURCEWIN32_A31ECBBA_1FF3_46D6_B8EA_5289C0FABFC9_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Application/DurationSeconds.hpp"

namespace Pomdog {
namespace Details {
namespace Win32 {

///@todo Run test and LINT
class TimeSourceWin32 {
public:
	TimeSourceWin32();

	TimePointSeconds Now() const;

private:
	double secondsPerTick;
};

}// namespace Win32
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_WIN32_TIMESOURCEWIN32_A31ECBBA_1FF3_46D6_B8EA_5289C0FABFC9_HPP)
