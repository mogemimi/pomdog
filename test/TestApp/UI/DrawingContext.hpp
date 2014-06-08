//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DRAWINGCONTEXT_693D88E1_20B1_45FB_A7CC_51EE5A85E085_HPP
#define POMDOG_DRAWINGCONTEXT_693D88E1_20B1_45FB_A7CC_51EE5A85E085_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

namespace Pomdog {
namespace UI {

class DrawingContext {
public:
	virtual ~DrawingContext() = default;

	virtual void DrawRectangle(Color const& color, Rectangle const& rectangle) = 0;
	virtual void DrawLine(Color const& color, float penSize, Point2D const& point1, Point2D const& point2) = 0;
	//void DrawEllipse();
	//void DrawText();
	//void DrawSprite();
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_DRAWINGCONTEXT_693D88E1_20B1_45FB_A7CC_51EE5A85E085_HPP)
