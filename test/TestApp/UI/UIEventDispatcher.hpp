//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_UIEVENTDISPATCHER_83873A91_6AAF_472C_9018_E882810EF741_HPP
#define POMDOG_UIEVENTDISPATCHER_83873A91_6AAF_472C_9018_E882810EF741_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <memory>
#include <vector>
#include <Pomdog/Pomdog.hpp>
#include <Pomdog/Utility/MakeUnique.hpp>
#include "UIElement.hpp"
#include "PointerPoint.hpp"
#include "PointerEventType.hpp"
#include "detail/SubscribeRequestDispatcher.hpp"

namespace Pomdog {
namespace UI {
namespace Details {

class UIEventConnection;

}// namespace Details


class UIEventDispatcher: public std::enable_shared_from_this<UIEventDispatcher> {
public:
	void UpdateChildren();

	void Touch(MouseState const& mouseState);
	
	void UpdateAnimation(DurationSeconds const& frameDuration);
	
	Details::UIEventConnection Connect(std::shared_ptr<UIElement> const& child);
	Details::UIEventConnection Connect(std::shared_ptr<UIElement> && child);

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

	Details::SubscribeRequestDispatcher subscribeRequests;
	std::vector<std::shared_ptr<UIElement>> children;
	std::unique_ptr<PointerState> pointerState;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_UIEVENTDISPATCHER_83873A91_6AAF_472C_9018_E882810EF741_HPP)
