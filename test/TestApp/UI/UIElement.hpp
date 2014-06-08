//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_UIELEMENT_92B47634_F035_41EC_A03C_C117576A4C75_HPP
#define POMDOG_UIELEMENT_92B47634_F035_41EC_A03C_C117576A4C75_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>

namespace Pomdog {
namespace UI {

class DrawingContext;
class PointerPoint;


class UIElement {
public:
	Rectangle bounds;
	float drawOrder;
	
	virtual ~UIElement() = default;
	
	bool Intersects(Point2D const& position)
	{
		return bounds.Intersects(position);
	}
	
	virtual void OnPointerCanceled(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerCaptureLost(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerEntered(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerExited(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerMoved(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerPressed(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerReleased(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerWheelChanged(PointerPoint const& pointerPoint) = 0;
	
	virtual void Draw(DrawingContext & drawingContext) = 0;
	
	virtual void UpdateAnimation(DurationSeconds const& frameDuration) = 0;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_UIELEMENT_92B47634_F035_41EC_A03C_C117576A4C75_HPP)
