//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PANEL_C95AA33F_4C44_4692_863A_0C270E741F09_HPP
#define POMDOG_PANEL_C95AA33F_4C44_4692_863A_0C270E741F09_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include "UIElement.hpp"

namespace Pomdog {
namespace UI {

class Panel: public UIElement {
public:
	virtual ~Panel() = default;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_PANEL_C95AA33F_4C44_4692_863A_0C270E741F09_HPP)
