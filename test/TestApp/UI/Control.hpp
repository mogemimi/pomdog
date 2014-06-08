//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_CONTROL_50F0BA0B_6FCF_4DB2_BF99_451A0CB2B4D2_HPP
#define POMDOG_CONTROL_50F0BA0B_6FCF_4DB2_BF99_451A0CB2B4D2_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include "UIElement.hpp"

namespace Pomdog {
namespace UI {

class Control: public UIElement {
public:
	virtual ~Control() = default;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_CONTROL_50F0BA0B_6FCF_4DB2_BF99_451A0CB2B4D2_HPP)
