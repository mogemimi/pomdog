//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_UIELEMENTHIERARCHY_ECF3A74B_7527_4C04_95B9_9EB2962F468B_HPP
#define POMDOG_UIELEMENTHIERARCHY_ECF3A74B_7527_4C04_95B9_9EB2962F468B_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "UIElement.hpp"
#include "UIEventDispatcher.hpp"
#include <Pomdog/Pomdog.hpp>
#include <algorithm>
#include <vector>
#include <memory>

namespace Pomdog {
namespace UI {

class DrawingContext;
class GameEditorView;
class UIView;

class UIElementHierarchy {
public:
	explicit UIElementHierarchy(std::shared_ptr<GameWindow> const& window);

	void Touch(MouseState const& mouseState);

	void UpdateAnimation(Duration const& frameDuration);

	void AddChild(std::shared_ptr<UIView> const& node);

	void AddChild(std::shared_ptr<UIView> && node);

	void Draw(DrawingContext & drawingContext);

	void RenderSizeChanged(std::uint32_t width, std::uint32_t height);

	std::shared_ptr<UIEventDispatcher> Dispatcher() const;

private:
	std::shared_ptr<UIEventDispatcher> dispatcher;
	std::shared_ptr<GameEditorView> root;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_UIELEMENTHIERARCHY_ECF3A74B_7527_4C04_95B9_9EB2962F468B_HPP)
