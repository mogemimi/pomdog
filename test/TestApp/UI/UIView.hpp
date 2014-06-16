//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_UIVIEW_6665F0EE_DCD8_4BFE_B293_1F0F83957750_HPP
#define POMDOG_UIVIEW_6665F0EE_DCD8_4BFE_B293_1F0F83957750_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include "UIElement.hpp"

namespace Pomdog {
namespace UI {

//using UIElementCollection = std::list<std::shared_ptr<UIElement>>;

class UIView: public UIElement {
public:
	UIView(Matrix3x2 const& transform, std::uint32_t widthIn, std::uint32_t heightIn);
	
	Matrix3x2 Transform() const override final { return renderTransform; }
	void Transform(Matrix3x2 const& matrix) override final { this->renderTransform = matrix; }
	void Transform(Matrix3x2 && matrix) override final { this->renderTransform = std::move(matrix); }
	//Vector2 Origin() const override final { return origin; }
	
	std::weak_ptr<UIElement const> Parent() const override final;
	std::weak_ptr<UIElement> Parent() override final;
	
	void Parent(std::shared_ptr<UIElement> const& parentIn)
	{
		this->parent = parentIn;
	}

	void MarkParentTransformDirty() override final;
	Matrix3x2 GlobalTransform() override final;
	
	std::uint16_t Width() const override final { return width; }
	std::uint16_t Height() const override final { return height; }

	void Width(std::uint16_t widthIn) { this->width = widthIn; }
	void Height(std::uint16_t heightIn) { this->height = heightIn; }
	
	bool SizeToFitContent() const override { return false; }
	
	void OnPointerCanceled(PointerPoint const& pointerPoint) override {}
	
	void OnPointerCaptureLost(PointerPoint const& pointerPoint) override {}
	
	void OnPointerWheelChanged(PointerPoint const& pointerPoint) override {}
	
	void OnPointerEntered(PointerPoint const& pointerPoint) override {}
	
	void OnPointerExited(PointerPoint const& pointerPoint) override {}
	
	void OnPointerPressed(PointerPoint const& pointerPoint) override {}
	
	void OnPointerMoved(PointerPoint const& pointerPoint) override {}
	
	void OnPointerReleased(PointerPoint const& pointerPoint) override {}
	
	void OnRenderSizeChanged(std::uint32_t, std::uint32_t) override {}
	
	void Draw(DrawingContext & drawingContext) override {}
	
	void UpdateAnimation(DurationSeconds const& frameDuration) override {}

private:
	Matrix3x2 renderTransform;
	//Vector2 origin;
	Matrix3x2 parentTransform;
	std::weak_ptr<UIElement> parent;
	bool isParentTransformDirty;
	std::uint16_t height;
	std::uint16_t width;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_UIVIEW_6665F0EE_DCD8_4BFE_B293_1F0F83957750_HPP)
