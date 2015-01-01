//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_TOUCHLOCATIONSTATE_C896CADD_F319_459F_A707_9504BEF69CFB_HPP
#define POMDOG_TOUCHLOCATIONSTATE_C896CADD_F319_459F_A707_9504BEF69CFB_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class TouchLocationState: std::uint8_t {
	Invalid,
	Moved,
	Pressed,
	Released,
};

}// namespace Pomdog

#endif // !defined(POMDOG_TOUCHLOCATIONSTATE_C896CADD_F319_459F_A707_9504BEF69CFB_HPP)
