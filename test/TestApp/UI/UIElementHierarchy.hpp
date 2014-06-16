//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_UIELEMENTHIERARCHY_ECF3A74B_7527_4C04_95B9_9EB2962F468B_HPP
#define POMDOG_UIELEMENTHIERARCHY_ECF3A74B_7527_4C04_95B9_9EB2962F468B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <algorithm>
#include <Pomdog/Pomdog.hpp>
#include <Pomdog/Utility/MakeUnique.hpp>
#include "UIElement.hpp"
#include "PointerPoint.hpp"
#include "PointerEventType.hpp"


namespace Pomdog {
namespace UI {

class DrawingContext;


class UIElementHierarchy {
public:
	void Touch(MouseState const& mouseState);
	
	void UpdateAnimation(DurationSeconds const& frameDuration);
	
	void AddChild(std::shared_ptr<UI::UIElement> const& node);
	
	void AddChild(std::shared_ptr<UI::UIElement> && node);

	void Draw(DrawingContext & drawingContext);
	
	void RenderSizeChanged(std::uint32_t width, std::uint32_t height);
	
private:
	std::shared_ptr<UIElement> Find(Point2D const& position);
	
	void PointerEntered(Point2D const& position, MouseState const& mouseState, std::shared_ptr<UI::UIElement> const& node);
	
	void PointerExited(Point2D const& position);
		
	void PointerPressed(Point2D const& position);
	
	void PointerMoved(Point2D const& position);
	
	void PointerReleased(Point2D const& position);
	
	Optional<UI::PointerMouseEvent> FindPointerMouseEvent(MouseState const& mouseState) const;

	ButtonState CheckMouseButton(MouseState const& mouseState, UI::PointerMouseEvent const& pointerMouseEvent) const;

	void Sort();

private:
	struct PointerState {
		std::shared_ptr<UI::UIElement> focusedElement;
		UI::PointerPoint pointerPoint;
		std::int32_t PrevScrollWheel;
	};

	std::unique_ptr<PointerState> pointerState;
	std::vector<std::shared_ptr<UI::UIElement>> children;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_UIELEMENTHIERARCHY_ECF3A74B_7527_4C04_95B9_9EB2962F468B_HPP)
