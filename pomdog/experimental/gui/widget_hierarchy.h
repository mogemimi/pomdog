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
} // namespace pomdog::gui

namespace pomdog::gui {

class WidgetHierarchy final {
public:
    WidgetHierarchy(
        const std::shared_ptr<GameWindow>& window,
        const std::shared_ptr<Keyboard>& keyboard);

    void touch(const MouseState& mouseState);

    void update();

    void updateAnimation(const Duration& frameDuration);

    void addChild(const std::shared_ptr<Widget>& child);

    void removeChild(const std::weak_ptr<Widget>& child);

    [[nodiscard]] bool
    contains(const std::shared_ptr<Widget>& child) const;

    void draw(DrawingContext& drawingContext);

    void renderSizeChanged(int width, int height);

    [[nodiscard]] std::shared_ptr<UIEventDispatcher>
    getDispatcher() const;

private:
    std::vector<std::shared_ptr<Widget>> subscribeRequests_;
    std::vector<std::shared_ptr<Widget>> children_;
    std::shared_ptr<UIEventDispatcher> dispatcher_;
    ScopedConnection addContextMenuConnection_;
    ScopedConnection removeContextMenuConnection_;
    ScopedConnection clientSizeChangedConnection_;
    int viewportHeight_;
};

} // namespace pomdog::gui
