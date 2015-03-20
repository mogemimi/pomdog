//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_UIEVENTDISPATCHER_83873A91_6AAF_472C_9018_E882810EF741_HPP
#define POMDOG_UIEVENTDISPATCHER_83873A91_6AAF_472C_9018_E882810EF741_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "UIView.hpp"
#include "PointerPoint.hpp"
#include "PointerEventType.hpp"
#include "detail/SubscribeRequestDispatcher.hpp"
#include <Pomdog/Pomdog.hpp>
#include <cstdint>
#include <memory>
#include <vector>

namespace Pomdog {
namespace UI {
namespace Detail {

class UIEventConnection;

}// namespace Detail


class UIEventDispatcher: public std::enable_shared_from_this<UIEventDispatcher> {
public:
	explicit UIEventDispatcher(std::shared_ptr<GameWindow> const& window);

	void UpdateChildren();

	void Touch(MouseState const& mouseState);

	void UpdateAnimation(Duration const& frameDuration);

	Detail::UIEventConnection Connect(std::shared_ptr<UIView> const& child);
	Detail::UIEventConnection Connect(std::shared_ptr<UIView> && child);

private:
	std::shared_ptr<UIView> Find(Point2D const& position);

	void PointerEntered(Point2D const& position, MouseState const& mouseState, std::shared_ptr<UIView> const& node);

	void PointerExited(Point2D const& position);

	void PointerPressed(Point2D const& position);

	void PointerMoved(Point2D const& position);

	void PointerReleased(Point2D const& position);

	Optional<PointerMouseEvent> FindPointerMouseEvent(MouseState const& mouseState) const;

	ButtonState CheckMouseButton(MouseState const& mouseState, PointerMouseEvent const& pointerMouseEvent) const;

	void Sort();

private:
	struct PointerState {
		std::shared_ptr<UIView> focusedElement;
		PointerPoint pointerPoint;
		std::int32_t PrevScrollWheel;
	};

	typedef Detail::SubscribeRequestDispatcher<UIView> SubscribeRequestDispatcherType;
	SubscribeRequestDispatcherType subscribeRequests;
	std::vector<std::shared_ptr<UIView>> children;
	std::unique_ptr<PointerState> pointerState;
	std::shared_ptr<GameWindow> window;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_UIEVENTDISPATCHER_83873A91_6AAF_472C_9018_E882810EF741_HPP)
