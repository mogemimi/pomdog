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
#include <vector>
#include <memory>
#include <Pomdog/Pomdog.hpp>
#include <Pomdog/Utility/MakeUnique.hpp>
#include "UIElement.hpp"
#include "UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {

class DrawingContext;
class GameEditorView;
class UIView;

class UIElementHierarchy {
public:
	UIElementHierarchy();

	void Touch(MouseState const& mouseState);
	
	void UpdateAnimation(DurationSeconds const& frameDuration);
	
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
