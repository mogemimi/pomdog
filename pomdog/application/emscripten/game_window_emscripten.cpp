// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/emscripten/game_window_emscripten.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/application/mouse_cursor.h"
#include "pomdog/application/window_mode.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::emscripten {
namespace {

[[nodiscard]] std::string_view
toCSSCursorName(MouseCursor cursor)
{
    using namespace std::string_view_literals;
    switch (cursor) {
    case MouseCursor::Arrow:
        return "default"sv;
    case MouseCursor::IBeam:
        return "text"sv;
    case MouseCursor::PointingHand:
        return "pointer"sv;
    case MouseCursor::ResizeVertical:
        return "ns-resize"sv;
    case MouseCursor::ResizeHorizontal:
        return "ew-resize"sv;
    }
    return "default"sv;
}

void setCanvasCursor(const std::string& canvasSelector, std::string_view cursorName)
{
    auto script = format(
        "var c = document.querySelector('{}'); if (c) {{ c.style.cursor = '{}'; }}",
        canvasSelector,
        cursorName);

    emscripten_run_script(script.c_str());
}

class GameWindowEmscriptenImpl final : public GameWindowEmscripten {
private:
    std::shared_ptr<SystemEventQueue> eventQueue_;
    std::string targetCanvas_ = {};
    std::string title_ = {};
    Rect2D clientBounds_ = {};
    Rect2D savedCanvasBounds_ = {};
    MouseCursor mouseCursor_ = MouseCursor::Arrow;
    WindowMode windowMode_ = WindowMode::Windowed;
    WindowMode windowModeBeforeFullscreen_ = WindowMode::Windowed;
    bool allowUserResizing_ = false;
    bool isMouseCursorVisible_ = true;

public:
    ~GameWindowEmscriptenImpl() override
    {
        // NOTE: Unregister fullscreen change callback.
        if (!targetCanvas_.empty()) {
            emscripten_set_fullscreenchange_callback(targetCanvas_.c_str(), nullptr, EM_FALSE, nullptr);
        }
    }

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::string& targetCanvas,
        int width,
        int height,
        const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept;

    bool
    getAllowUserResizing() const override
    {
        return allowUserResizing_;
    }

    void setAllowUserResizing(bool allowResizing) override
    {
        allowUserResizing_ = allowResizing;
    }

    std::string
    getTitle() const override
    {
        return title_;
    }

    void setTitle(const std::string& titleIn) override
    {
        title_ = titleIn;
    }

    Rect2D
    getClientBounds() const override
    {
        return clientBounds_;
    }

    void setClientBounds(const Rect2D& clientBoundsIn) override
    {
        clientBounds_ = clientBoundsIn;
        emscripten_set_canvas_element_size(
            targetCanvas_.c_str(),
            clientBounds_.width,
            clientBounds_.height);
        clientSizeChanged(clientBounds_.width, clientBounds_.height);
    }

    bool
    isMouseCursorVisible() const override
    {
        return isMouseCursorVisible_;
    }

    void setMouseCursorVisible(bool visible) override
    {
        isMouseCursorVisible_ = visible;

        if (isMouseCursorVisible_) {
            setCanvasCursor(targetCanvas_, toCSSCursorName(mouseCursor_));
        }
        else {
            setCanvasCursor(targetCanvas_, "none");
        }
    }

    void setMouseCursor(MouseCursor cursor) override
    {
        mouseCursor_ = cursor;

        if (isMouseCursorVisible_) {
            setCanvasCursor(targetCanvas_, toCSSCursorName(mouseCursor_));
        }
    }

    WindowMode
    getWindowMode() const noexcept override
    {
        return windowMode_;
    }

    [[nodiscard]] std::unique_ptr<Error>
    setWindowMode(WindowMode windowMode) noexcept override;

    const std::string&
    getTargetCanvas() const noexcept override
    {
        return targetCanvas_;
    }

    /// Called by the fullscreen change callback when the browser exits fullscreen
    /// (e.g., the user presses Escape).
    void applyWindowModeFromBrowser(WindowMode newMode) noexcept
    {
        if (windowMode_ == newMode) {
            return;
        }
        if (newMode == WindowMode::Windowed) {
            // NOTE: Restore canvas size to what it was before fullscreen.
            const auto restoreMode = windowModeBeforeFullscreen_;
            clientBounds_.width = savedCanvasBounds_.width;
            clientBounds_.height = savedCanvasBounds_.height;
            emscripten_set_canvas_element_size(
                targetCanvas_.c_str(),
                savedCanvasBounds_.width,
                savedCanvasBounds_.height);

            // NOTE: If we were in soft-fullscreen before going fullscreen, re-enter it.
            if (restoreMode == WindowMode::BrowserSoftFullscreen) {
                windowMode_ = WindowMode::Windowed; // temporarily to allow setWindowMode
                if (setWindowMode(WindowMode::BrowserSoftFullscreen) != nullptr) {
                    // NOTE: Failed - stay windowed.
                    windowMode_ = WindowMode::Windowed;
                }
                // NOTE: setWindowMode already enqueued the event.
                return;
            }
        }
        windowMode_ = newMode;
        if (eventQueue_) {
            eventQueue_->enqueue(SystemEvent{
                .kind = SystemEventKind::WindowModeChangedEvent,
                .data = WindowModeChangedEvent{.windowMode = newMode},
            });
        }
    }
};

EM_BOOL
onFullscreenChange(int eventType, const EmscriptenFullscreenChangeEvent* event, void* userData) noexcept
{
    if (eventType != EMSCRIPTEN_EVENT_FULLSCREENCHANGE) {
        return EM_TRUE;
    }

    auto* window = static_cast<GameWindowEmscriptenImpl*>(userData);
    POMDOG_ASSERT(window != nullptr);

    const auto newMode = (event->isFullscreen == EM_TRUE)
                             ? WindowMode::Fullscreen
                             : WindowMode::Windowed;

    window->applyWindowModeFromBrowser(newMode);

    return EM_TRUE;
}

[[nodiscard]] std::unique_ptr<Error>
GameWindowEmscriptenImpl::initialize(
    const std::string& targetCanvas,
    int width,
    int height,
    const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept
{
    eventQueue_ = eventQueue;
    targetCanvas_ = targetCanvas;

    if (width > 0 && height > 0) {
        emscripten_set_canvas_element_size(targetCanvas_.c_str(), width, height);
    }

    f64 cssWidth = 0.0;
    f64 cssHeight = 0.0;
    emscripten_get_element_css_size(targetCanvas_.c_str(), &cssWidth, &cssHeight);

    clientBounds_.x = 0;
    clientBounds_.y = 0;
    clientBounds_.width = static_cast<i32>(cssWidth);
    clientBounds_.height = static_cast<i32>(cssHeight);
    savedCanvasBounds_ = clientBounds_;

    // NOTE: Detect initial fullscreen state.
    {
        EmscriptenFullscreenChangeEvent status;
        emscripten_get_fullscreen_status(&status);
        windowMode_ = (status.isFullscreen == EM_TRUE) ? WindowMode::Fullscreen : WindowMode::Windowed;
    }

    // NOTE: Register fullscreen change callback to detect browser-level transitions.
    emscripten_set_fullscreenchange_callback(
        targetCanvas_.c_str(),
        this,
        EM_FALSE,
        onFullscreenChange);

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
GameWindowEmscriptenImpl::setWindowMode(WindowMode windowMode) noexcept
{
    if (windowMode_ == windowMode) {
        return nullptr;
    }

    // NOTE: Exit current mode first.
    switch (windowMode_) {
    case WindowMode::Fullscreen: {
        emscripten_exit_fullscreen();
        windowModeBeforeFullscreen_ = WindowMode::Windowed;
        // NOTE: Restore canvas size.
        clientBounds_.width = savedCanvasBounds_.width;
        clientBounds_.height = savedCanvasBounds_.height;
        emscripten_set_canvas_element_size(targetCanvas_.c_str(), savedCanvasBounds_.width, savedCanvasBounds_.height);
        break;
    }
    case WindowMode::BrowserSoftFullscreen: {
        emscripten_exit_soft_fullscreen();
        // NOTE: Restore canvas size.
        clientBounds_.width = savedCanvasBounds_.width;
        clientBounds_.height = savedCanvasBounds_.height;
        emscripten_set_canvas_element_size(targetCanvas_.c_str(), savedCanvasBounds_.width, savedCanvasBounds_.height);
        break;
    }
    case WindowMode::Windowed:
    case WindowMode::Maximized:
    case WindowMode::BorderlessWindowed:
        break;
    }

    switch (windowMode) {
    case WindowMode::Windowed: {
        if (eventQueue_) {
            eventQueue_->enqueue(SystemEvent{
                .kind = SystemEventKind::WindowModeChangedEvent,
                .data = WindowModeChangedEvent{.windowMode = WindowMode::Windowed},
            });
        }
        break;
    }
    case WindowMode::BrowserSoftFullscreen: {
        // NOTE: Save canvas size before entering soft fullscreen.
        savedCanvasBounds_ = clientBounds_;

        EmscriptenFullscreenStrategy strategy;
        strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_STRETCH;
        strategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
        strategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
        strategy.canvasResizedCallback = [](int /*eventType*/, const void* /*reserved*/, void* userData) -> EM_BOOL {
            auto* self = static_cast<GameWindowEmscriptenImpl*>(userData);
            f64 cssWidth = 0.0;
            f64 cssHeight = 0.0;
            emscripten_get_element_css_size(self->targetCanvas_.c_str(), &cssWidth, &cssHeight);
            const auto newWidth = static_cast<i32>(cssWidth);
            const auto newHeight = static_cast<i32>(cssHeight);
            if (newWidth > 0 && newHeight > 0) {
                emscripten_set_canvas_element_size(self->targetCanvas_.c_str(), newWidth, newHeight);
                self->clientBounds_.width = newWidth;
                self->clientBounds_.height = newHeight;
                self->clientSizeChanged(newWidth, newHeight);
                if (self->eventQueue_) {
                    self->eventQueue_->enqueue(SystemEvent{
                        .kind = SystemEventKind::ViewNeedsUpdateSurfaceEvent,
                        .data = {},
                    });
                }
            }
            return EM_TRUE;
        };
        strategy.canvasResizedCallbackUserData = this;
        strategy.canvasResizedCallbackTargetThread = 0;

        emscripten_enter_soft_fullscreen(targetCanvas_.c_str(), &strategy);

        if (eventQueue_) {
            eventQueue_->enqueue(SystemEvent{
                .kind = SystemEventKind::WindowModeChangedEvent,
                .data = WindowModeChangedEvent{.windowMode = WindowMode::BrowserSoftFullscreen},
            });
        }
        break;
    }
    case WindowMode::Fullscreen: {
        // NOTE: Save canvas size and previous mode before going fullscreen.
        savedCanvasBounds_ = clientBounds_;
        windowModeBeforeFullscreen_ = windowMode_;

        // HACK: https://github.com/emscripten-core/emscripten/issues/5124
        EM_ASM(JSEvents.inEventHandler = true);
        EM_ASM(JSEvents.currentEventHandler = {allowsDeferredCalls : true});

        EmscriptenFullscreenStrategy strategy;
        strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_STRETCH;
        strategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
        strategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
        strategy.canvasResizedCallback = [](int /*eventType*/, const void* /*reserved*/, void* userData) -> EM_BOOL {
            auto* self = static_cast<GameWindowEmscriptenImpl*>(userData);
            int w = 0;
            int h = 0;
            emscripten_get_canvas_element_size(self->targetCanvas_.c_str(), &w, &h);
            if (w > 0 && h > 0 && (self->clientBounds_.width != w || self->clientBounds_.height != h)) {
                self->clientBounds_.width = w;
                self->clientBounds_.height = h;
                self->clientSizeChanged(w, h);
                if (self->eventQueue_) {
                    self->eventQueue_->enqueue(SystemEvent{
                        .kind = SystemEventKind::ViewNeedsUpdateSurfaceEvent,
                        .data = {},
                    });
                }
            }
            return EM_TRUE;
        };
        strategy.canvasResizedCallbackUserData = this;
        strategy.canvasResizedCallbackTargetThread = 0;

        emscripten_request_fullscreen_strategy(targetCanvas_.c_str(), EM_FALSE, &strategy);
        // NOTE: WindowModeChangedEvent will be sent by the onFullscreenChange callback
        //       when the browser confirms the fullscreen transition.
        break;
    }
    case WindowMode::BorderlessWindowed:
    case WindowMode::Maximized: {
        return errors::make("WindowMode is not supported on Emscripten.");
    }
    }

    windowMode_ = windowMode;
    return nullptr;
}

} // namespace

GameWindowEmscripten::GameWindowEmscripten() noexcept = default;

GameWindowEmscripten::~GameWindowEmscripten() noexcept = default;

std::tuple<std::shared_ptr<GameWindowEmscripten>, std::unique_ptr<Error>>
GameWindowEmscripten::create(
    const std::string& targetCanvas,
    int width,
    int height,
    const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept
{
    auto window = std::make_shared<GameWindowEmscriptenImpl>();
    if (auto err = window->initialize(targetCanvas, width, height, eventQueue); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(window), nullptr);
}

} // namespace pomdog::detail::emscripten
