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
	UIView(Matrix3x2 const& transform, std::uint32_t width, std::uint32_t height);
	
	Matrix3x2 Transform() const override final;
	void Transform(Matrix3x2 const& matrix) override final;
	void Transform(Matrix3x2 && matrix) override final;
	//Vector2 Origin() const override final { return origin; }

	std::weak_ptr<UIElement const> Parent() const override final;
	std::weak_ptr<UIElement> Parent() override final;
	
	void Parent(std::weak_ptr<UIElement> const& parentIn);

	void MarkParentTransformDirty() override final;
	Matrix3x2 GlobalTransform() override final;
	
	void MarkParentDrawOrderDirty() override final;
	std::int32_t GlobalDrawOrder() override final;
	void DrawOrder(std::int32_t drawOrder) override final;
	std::int32_t DrawOrder() const override final;

	std::uint16_t Width() const override final { return width; }
	std::uint16_t Height() const override final { return height; }

	void Width(std::uint16_t widthIn) { this->width = widthIn; }
	void Height(std::uint16_t heightIn) { this->height = heightIn; }

	UI::HorizontalAlignment HorizontalAlignment() const override { return UI::HorizontalAlignment::Stretch; }
	UI::VerticalAlignment VerticalAlignment() const override { return UI::VerticalAlignment::Stretch; }

	bool SizeToFitContent() const override { return false; }

	std::weak_ptr<UIEventDispatcher> Dispatcher() const override
	{
		POMDOG_ASSERT(false);
		return {};
	}
	
	void OnParentChanged() override {}

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
	std::uint32_t parentDrawOrder;
	std::uint32_t drawOrder;
	std::uint16_t height;
	std::uint16_t width;
	bool isParentTransformDirty;
	bool isParentDrawOrderDirty;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_UIVIEW_6665F0EE_DCD8_4BFE_B293_1F0F83957750_HPP)
