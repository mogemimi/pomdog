// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/signals/scoped_connection.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

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

} // namespace pomdog::gui
