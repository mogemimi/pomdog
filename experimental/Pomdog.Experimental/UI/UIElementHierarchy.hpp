// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

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
class UIElement;

class UIElementHierarchy {
public:
    explicit UIElementHierarchy(std::shared_ptr<GameWindow> const& window);

    void Touch(MouseState const& mouseState);

    void UpdateLayout();

    void UpdateAnimation(Duration const& frameDuration);

    void AddChild(std::shared_ptr<UIElement> const& child);

    void AddChild(std::shared_ptr<UIElement> && child);

    void Draw(DrawingContext & drawingContext);

    void RenderSizeChanged(std::uint32_t width, std::uint32_t height);

    std::shared_ptr<UIEventDispatcher> Dispatcher() const;

private:
    std::vector<std::shared_ptr<UIElement>> children;
    std::shared_ptr<UIEventDispatcher> dispatcher;
};

} // namespace UI
} // namespace Pomdog
