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

enum class HorizontalAlignment: std::uint8_t {
	Left,
	//Center,
	Stretch,
	//Right,
};

enum class VerticalAlignment: std::uint8_t {
	//Bottom,
	//Center,
	Stretch,
	Top,
};

class UIElement {
public:
	///@todo badcode
	float drawOrder;
	
	virtual ~UIElement() = default;

	virtual bool SizeToFitContent() const = 0;
	virtual Matrix3x2 Transform() const = 0;
	virtual void Transform(Matrix3x2 const& matrix) = 0;
	virtual void Transform(Matrix3x2 && matrix) = 0;
	virtual std::uint16_t Width() const = 0;
	virtual std::uint16_t Height() const = 0;
	
	virtual std::weak_ptr<UIElement const> Parent() const = 0;
	virtual std::weak_ptr<UIElement> Parent() = 0;
	
	virtual void MarkParentTransformDirty() = 0;
	virtual Matrix3x2 GlobalTransform() = 0;

	virtual UI::HorizontalAlignment HorizontalAlignment() const = 0;
	virtual UI::VerticalAlignment VerticalAlignment() const = 0;

//	virtual void Parent(std::weak_ptr<UIElement> const& parent) = 0;
//	virtual Thickness Padding() const = 0;
//	virtual Vector2 Origin() const = 0;
//	virtual Thickness Margin() const = 0;

	virtual void OnPointerCanceled(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerCaptureLost(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerEntered(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerExited(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerMoved(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerPressed(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerReleased(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnPointerWheelChanged(PointerPoint const& pointerPoint) = 0;
	
	virtual void OnRenderSizeChanged(std::uint32_t width, std::uint32_t height) = 0;
	
	virtual void Draw(DrawingContext & drawingContext) = 0;
	
	virtual void UpdateAnimation(DurationSeconds const& frameDuration) = 0;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_UIELEMENT_92B47634_F035_41EC_A03C_C117576A4C75_HPP)
