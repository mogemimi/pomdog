//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_MOUSEHELPER_8D10904C_EFC6_4894_90F7_63815BA05359_HPP
#define POMDOG_MOUSEHELPER_8D10904C_EFC6_4894_90F7_63815BA05359_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "MouseCursor.hpp"

namespace Pomdog {
namespace UI {

struct MouseHelper {
	static void ShowMouseCursor();
	
	static void HideMouseCursor();

	static void SetMouseCursor(MouseCursor cursor);
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_MOUSEHELPER_8D10904C_EFC6_4894_90F7_63815BA05359_HPP)
