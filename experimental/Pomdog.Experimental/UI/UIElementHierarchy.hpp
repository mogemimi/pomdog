// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/UIElement.hpp"
#include "Pomdog.Experimental/UI/UIEventDispatcher.hpp"
#include <Pomdog/Pomdog.hpp>
#include <algorithm>
#include <vector>
#include <memory>

namespace Pomdog {
namespace UI {

class DrawingContext;
class GameEditorView;
class UIElement;

class UIElementHierarchy final {
public:
    explicit UIElementHierarchy(const std::shared_ptr<GameWindow>& window);

    void Touch(const MouseState& mouseState);

    void UpdateAnimation(const Duration& frameDuration);

    void AddChild(const std::shared_ptr<UIElement>& child);

    void Draw(DrawingContext & drawingContext);

    void RenderSizeChanged(int width, int height);

    std::shared_ptr<UIEventDispatcher> GetDispatcher() const;

private:
    std::vector<std::shared_ptr<UIElement>> children;
    std::shared_ptr<UIEventDispatcher> dispatcher;
    int viewportHeight;
};

} // namespace UI
} // namespace Pomdog
