// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/GUI/Widget.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include <algorithm>
#include <memory>
#include <vector>

namespace Pomdog::GUI {

class DrawingContext;
class Widget;

class WidgetHierarchy final {
public:
    WidgetHierarchy(
        const std::shared_ptr<GameWindow>& window,
        const std::shared_ptr<Keyboard>& keyboard);

    void Touch(const MouseState& mouseState);

    void Update();

    void UpdateAnimation(const Duration& frameDuration);

    void AddChild(const std::shared_ptr<Widget>& child);

    void RemoveChild(const std::weak_ptr<Widget>& child);

    bool Contains(const std::shared_ptr<Widget>& child) const;

    void Draw(DrawingContext& drawingContext);

    void RenderSizeChanged(int width, int height);

    std::shared_ptr<UIEventDispatcher> GetDispatcher() const;

private:
    std::vector<std::shared_ptr<Widget>> subscribeRequests;
    std::vector<std::shared_ptr<Widget>> children;
    std::shared_ptr<UIEventDispatcher> dispatcher;
    ScopedConnection connection;
    int viewportHeight;
};

} // namespace Pomdog::GUI
