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

#include <cstdint>
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {
namespace UI {

enum class FontWeight: std::uint8_t {
    Light,
    Normal,
    Bold,
};

enum class FontSize: std::uint8_t {
    Small,
    Medium,
    Large,
};

class DrawingContext {
public:
	virtual ~DrawingContext() = default;

	virtual Matrix3x2 Top() const = 0;
	virtual void Push(Matrix3x2 const& matrix) = 0;
	virtual void Pop() = 0;

	virtual void DrawRectangle(Matrix3x2 const& transform, Color const& color, Rectangle const& rectangle) = 0;
	virtual void DrawLine(Matrix3x2 const& transform, Color const& color, float penSize, Point2D const& point1, Point2D const& point2) = 0;
	virtual void DrawString(Matrix3x2 const& transform, Color const& color,
		FontWeight fontWeight, FontSize fontSize, std::string const& text) = 0;
	//void DrawEllipse();
	//void DrawSprite();
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_DRAWINGCONTEXT_693D88E1_20B1_45FB_A7CC_51EE5A85E085_HPP)
