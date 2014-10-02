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

#include "HorizontalAlignment.hpp"
#include "VerticalAlignment.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {
namespace UI {

class DrawingContext;
class UIEventDispatcher;

class UIElement {
public:
	virtual ~UIElement() = default;
	
	virtual std::weak_ptr<UIElement const> Parent() const = 0;
	
	virtual std::weak_ptr<UIElement> Parent() = 0;
	
	virtual Matrix3x2 Transform() const = 0;
	
	virtual void Transform(Matrix3x2 const& matrix) = 0;
	
	virtual void Transform(Matrix3x2 && matrix) = 0;
	
	virtual void UpdateTransform() = 0;
	
	virtual void MarkParentTransformDirty() = 0;
	
	virtual void DrawOrder(std::int32_t drawOrder) = 0;
	
	virtual std::int32_t DrawOrder() const = 0;
	
	virtual Matrix3x2 GlobalTransform() const = 0;

	virtual void MarkParentDrawOrderDirty() = 0;
	
	virtual std::int32_t GlobalDrawOrder() = 0;

	virtual std::uint16_t Width() const = 0;
	
	virtual std::uint16_t Height() const = 0;
	
	virtual bool SizeToFitContent() const = 0;

	virtual HorizontalAlignment HorizontalAlignment() const = 0;
	
	virtual VerticalAlignment VerticalAlignment() const = 0;

	virtual std::weak_ptr<UIEventDispatcher> Dispatcher() const = 0;

//	virtual void Parent(std::weak_ptr<UIElement> const& parent) = 0;
//	virtual Thickness Padding() const = 0;
//	virtual Vector2 Origin() const = 0;
//	virtual Thickness Margin() const = 0;

	virtual void OnParentChanged() = 0;
	
	virtual void OnRenderSizeChanged(std::uint32_t width, std::uint32_t height) = 0;

	virtual void Draw(DrawingContext & drawingContext) = 0;
	
	virtual void UpdateAnimation(DurationSeconds const& frameDuration) = 0;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_UIELEMENT_92B47634_F035_41EC_A03C_C117576A4C75_HPP)
