//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_MOUSECURSOR_D7B37AAC_2F41_481D_9A63_BB34AAB1FDFE_HPP
#define POMDOG_MOUSECURSOR_D7B37AAC_2F41_481D_9A63_BB34AAB1FDFE_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class MouseCursor: std::uint8_t {
	Arrow,
	IBeam,
	PointingHand,
	ResizeVertical,
	ResizeHorizontal,
};

}// namespace Pomdog

#endif // !defined(POMDOG_MOUSECURSOR_D7B37AAC_2F41_481D_9A63_BB34AAB1FDFE_HPP)
