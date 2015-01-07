//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_UIEVENTLISTENER_6C74CE34_0376_4123_B183_0B11038A0D68_HPP
#define POMDOG_UIEVENTLISTENER_6C74CE34_0376_4123_B183_0B11038A0D68_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

namespace Pomdog {
namespace UI {

class PointerPoint;

class UIEventListener {
public:
	virtual ~UIEventListener() = default;

	virtual void OnPointerCanceled(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerCaptureLost(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerEntered(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerExited(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerMoved(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerPressed(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerReleased(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerWheelChanged(PointerPoint const& pointerPoint) = 0;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_UIEVENTLISTENER_6C74CE34_0376_4123_B183_0B11038A0D68_HPP)
