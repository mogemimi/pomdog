//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_TOGGLESWITCH_62F7E7F0_48AA_45A8_9010_1426B8EBD8A2_HPP
#define POMDOG_TOGGLESWITCH_62F7E7F0_48AA_45A8_9010_1426B8EBD8A2_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Control.hpp"

namespace Pomdog {
namespace UI {

class ToggleSwitch: public Control {
public:
	ToggleSwitch();
	
	bool IsOn() const;
	void IsOn(bool isOn);
	
	bool IsEnabled() const;
	void IsEnabled(bool isEnabled);

	UI::HorizontalAlignment HorizontalAlignment() const override { return UI::HorizontalAlignment::Left; }
	UI::VerticalAlignment VerticalAlignment() const override { return UI::VerticalAlignment::Top; }

	void OnPointerPressed(PointerPoint const& pointerPoint) override;

	void OnPointerReleased(PointerPoint const& pointerPoint) override;
	
	void Draw(DrawingContext & drawingContext) override;
	
private:
	bool isOn;
	bool isEnabled;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_TOGGLESWITCH_62F7E7F0_48AA_45A8_9010_1426B8EBD8A2_HPP)
