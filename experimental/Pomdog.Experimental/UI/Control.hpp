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

#include "UIView.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {
namespace UI {

class Control: public UIView {
public:
	virtual ~Control() = default;
	
	Control(Matrix3x2 const& transformIn, std::uint32_t widthIn, std::uint32_t heightIn)
		: UIView(transformIn, widthIn, heightIn)
	{}
	
//	UI::HorizontalAlignment HorizontalAlignment() const final override { return horizontalAlignment; }
//	UI::VerticalAlignment VerticalAlignment() const final override { return verticalAlignment; }
//
//	void HorizontalAlignment(UI::HorizontalAlignment alighnment) { this->horizontalAlignment = alighnment; }
//	void VerticalAlignment(UI::VerticalAlignment alighnment) { this->verticalAlignment = alighnment; }
//	
//private:
//	UI::HorizontalAlignment horizontalAlignment = UI::HorizontalAlignment::Stretch;
//	UI::VerticalAlignment verticalAlignment = UI::VerticalAlignment::Stretch;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_CONTROL_50F0BA0B_6FCF_4DB2_BF99_451A0CB2B4D2_HPP)
