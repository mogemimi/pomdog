// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/UI/UIElement.hpp"
#include "Pomdog/Experimental/UI/UIEventDispatcher.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include <algorithm>
#include <memory>
#include <vector>

namespace Pomdog::UI {

class DrawingContext;
class UIElement;

class UIElementHierarchy final {
public:
    UIElementHierarchy(
        const std::shared_ptr<GameWindow>& window,
        const std::shared_ptr<Keyboard>& keyboard);

    void Touch(const MouseState& mouseState);

    void UpdateAnimation(const Duration& frameDuration);

    void AddChild(const std::shared_ptr<UIElement>& child);

    void Draw(DrawingContext& drawingContext);

    void RenderSizeChanged(int width, int height);

    std::shared_ptr<UIEventDispatcher> GetDispatcher() const;

private:
    std::vector<std::shared_ptr<UIElement>> children;
    std::shared_ptr<UIEventDispatcher> dispatcher;
    ScopedConnection connection;
    int viewportHeight;
};

} // namespace Pomdog::UI
