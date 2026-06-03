// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/memory/raw_ptr.h"
#include "pomdog/signals/scoped_connection.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Touchscreen;
} // namespace pomdog

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

    /// Dispatches pointer input to the widget tree.
    ///
    /// Combines the mouse and the touchscreen into a single pointer; a pressed
    /// touch acts as the primary (left) button. Pass a null `touchscreen` on
    /// platforms without touch input.
    void touch(const Mouse& mouse, raw_ptr<const Touchscreen> touchscreen);

    /// Sets the UI scale.
    ///
    /// The GUI is laid out in logical pixels divided by `uiScale`, so a value
    /// greater than 1 enlarges the whole UI relative to the screen. This is
    /// useful for keeping a UI readable on small high-density displays. It is
    /// distinct from per-widget scaling. Defaults to 1.0 (one GUI unit equals
    /// one logical pixel). The matching render scale to pass to
    /// `DrawingContext::reset()` is `pixelRatio * uiScale`.
    void setUIScale(f32 uiScale);

    /// Returns the current UI scale.
    [[nodiscard]] f32
    getUIScale() const noexcept;

    void update();

    void updateAnimation(const Duration& frameDuration);

    void addChild(const std::shared_ptr<Widget>& child);

    void removeChild(const std::weak_ptr<Widget>& child);

    [[nodiscard]] bool
    contains(const std::shared_ptr<Widget>& child) const;

    void draw(DrawingContext& drawingContext);

    void renderSizeChanged(i32 width, i32 height);

    [[nodiscard]] std::shared_ptr<UIEventDispatcher>
    getDispatcher() const;

private:
    std::vector<std::shared_ptr<Widget>> subscribeRequests_;
    std::vector<std::shared_ptr<Widget>> children_;
    std::shared_ptr<UIEventDispatcher> dispatcher_;
    std::shared_ptr<GameWindow> window_;
    ScopedConnection addContextMenuConnection_;
    ScopedConnection removeContextMenuConnection_;
    ScopedConnection clientSizeChangedConnection_;
    i32 viewportHeight_ = 0;
    f32 uiScale_ = 1.0f;
};

} // namespace pomdog::gui
