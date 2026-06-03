// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/emscripten/game_window_emscripten.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/application/display_metrics.h"
#include "pomdog/application/high_dpi_settings.h"
#include "pomdog/application/mouse_cursor.h"
#include "pomdog/application/window_mode.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <cmath>
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

[[nodiscard]] f32
queryRawPixelRatioFromBrowser() noexcept
{
    return static_cast<f32>(emscripten_get_device_pixel_ratio());
}

class GameWindowEmscriptenImpl final : public GameWindowEmscripten {
private:
    std::shared_ptr<SystemEventQueue> eventQueue_;
    std::string targetCanvas_ = {};
    std::string title_ = {};
    Rect2D savedCanvasBounds_ = {};
    MouseCursor mouseCursor_ = MouseCursor::Arrow;
    WindowMode windowMode_ = WindowMode::Windowed;
    WindowMode windowModeBeforeFullscreen_ = WindowMode::Windowed;
    HighDPISettings highDPI_ = {};

    // NOTE: `committedMetrics_` is the snapshot exposed via getClientBounds(),
    // getPixelRatio() and getDisplayMetrics(). It is updated only at frame
    // boundary via commitDisplayMetricsIfChanged().
    // `platformLiveMetrics_` mirrors the latest values observed from the
    // browser (CSS size, devicePixelRatio).
    DisplayMetrics committedMetrics_ = {};
    DisplayMetrics platformLiveMetrics_ = {};

    bool allowUserResizing_ = false;
    bool isMouseCursorVisible_ = true;

    // NOTE: Pending requests - set by `request...()` and applied at the next
    //       `applyPendingWindowRequests()` call at the frame boundary.
    std::optional<WindowMode> pendingWindowMode_;
    std::optional<Rect2D> pendingClientBounds_;
    std::optional<std::string> pendingTitle_;
    std::optional<bool> pendingAllowUserResizing_;

public:
    ~GameWindowEmscriptenImpl() override
    {
        // NOTE: Unregister fullscreen change callback.
        if (!targetCanvas_.empty()) {
            emscripten_set_fullscreenchange_callback(targetCanvas_.c_str(), nullptr, EM_FALSE, nullptr);
            emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, nullptr);
        }
    }

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::string& targetCanvas,
        i32 clientWidth,
        i32 clientHeight,
        const HighDPISettings& highDPIIn,
        const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept;

    bool
    getAllowUserResizing() const override
    {
        return allowUserResizing_;
    }

    void requestAllowUserResizing(bool allowResizing) override
    {
        pendingAllowUserResizing_ = allowResizing;
    }

    std::string
    getTitle() const override
    {
        return title_;
    }

    void requestTitle(const std::string& titleIn) override
    {
        pendingTitle_ = titleIn;
    }

    Rect2D
    getClientBounds() const override
    {
        return committedMetrics_.clientBounds;
    }

    [[nodiscard]] std::unique_ptr<Error>
    requestClientBounds(const Rect2D& clientBoundsIn) noexcept override
    {
        if (clientBoundsIn.width <= 0 || clientBoundsIn.height <= 0) {
            return errors::make("requestClientBounds: width and height must be positive");
        }
        pendingClientBounds_ = clientBoundsIn;
        return nullptr;
    }

    [[nodiscard]] std::optional<Rect2D>
    getPendingClientBounds() const noexcept override
    {
        return pendingClientBounds_;
    }

    [[nodiscard]] f32
    getPixelRatio() const noexcept override
    {
        return committedMetrics_.pixelRatio;
    }

    [[nodiscard]] DisplayMetrics
    getDisplayMetrics() const noexcept override
    {
        return committedMetrics_;
    }

    [[nodiscard]] std::optional<DisplayMetrics>
    commitDisplayMetricsIfChanged() noexcept override
    {
        refreshPlatformLiveMetrics();
        if (platformLiveMetrics_ == committedMetrics_) {
            return std::nullopt;
        }
        committedMetrics_ = platformLiveMetrics_;
        return committedMetrics_;
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
    requestWindowMode(WindowMode windowMode) noexcept override
    {
        // NOTE: Validate immediately for modes that are never supported on this platform.
        if (windowMode == WindowMode::BorderlessWindowed || windowMode == WindowMode::Maximized) {
            return errors::make("WindowMode is not supported on Emscripten.");
        }
        pendingWindowMode_ = windowMode;
        return nullptr;
    }

    [[nodiscard]] std::optional<WindowMode>
    getPendingWindowMode() const noexcept override
    {
        return pendingWindowMode_;
    }

    void
    applyPendingWindowRequests() noexcept override
    {
        // NOTE: Apply window-mode first.
        if (pendingWindowMode_.has_value()) {
            [[maybe_unused]] auto err = applyWindowMode(*pendingWindowMode_);
            pendingWindowMode_ = std::nullopt;
        }

        if (pendingClientBounds_.has_value()) {
            if (windowMode_ == WindowMode::Windowed || windowMode_ == WindowMode::BrowserSoftFullscreen) {
                applyClientBounds(*pendingClientBounds_);
            }
            pendingClientBounds_ = std::nullopt;
        }

        if (pendingTitle_.has_value()) {
            title_ = *pendingTitle_;
            pendingTitle_ = std::nullopt;
        }

        if (pendingAllowUserResizing_.has_value()) {
            allowUserResizing_ = *pendingAllowUserResizing_;
            pendingAllowUserResizing_ = std::nullopt;
        }
    }

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
            // NOTE: Restore canvas size to what it was before fullscreen (logical pixels).
            const auto restoreMode = windowModeBeforeFullscreen_;
            applyCanvasBackingSize(savedCanvasBounds_.width, savedCanvasBounds_.height, platformLiveMetrics_.pixelRatio);

            // NOTE: If we were in soft-fullscreen before going fullscreen, re-enter it.
            if (restoreMode == WindowMode::BrowserSoftFullscreen) {
                windowMode_ = WindowMode::Windowed; // temporarily to allow applyWindowMode
                if (applyWindowMode(WindowMode::BrowserSoftFullscreen) != nullptr) {
                    // NOTE: Failed - stay windowed.
                    windowMode_ = WindowMode::Windowed;
                }
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

    /// Called by the browser resize callback. Refreshes the platform-live
    /// metrics by re-reading the CSS size and `devicePixelRatio`.
    void onBrowserResized() noexcept
    {
        refreshPlatformLiveMetrics();
        if (eventQueue_) {
            eventQueue_->enqueue(SystemEvent{
                .kind = SystemEventKind::ViewNeedsUpdateSurfaceEvent,
                .data = {},
            });
        }
    }

private:
    /// Reads the current CSS size and devicePixelRatio from the browser and
    /// updates the canvas backing store and `platformLiveMetrics_`.
    void refreshPlatformLiveMetrics() noexcept
    {
        f64 cssWidth = 0.0;
        f64 cssHeight = 0.0;
        emscripten_get_element_css_size(targetCanvas_.c_str(), &cssWidth, &cssHeight);

        const i32 logicalW = static_cast<i32>(std::lround(cssWidth));
        const i32 logicalH = static_cast<i32>(std::lround(cssHeight));

        const f32 rawRatio = queryRawPixelRatioFromBrowser();
        const f32 effRatio = computeEffectivePixelRatio(highDPI_, rawRatio);

        const i32 physW = static_cast<i32>(std::lround(static_cast<f32>(logicalW) * effRatio));
        const i32 physH = static_cast<i32>(std::lround(static_cast<f32>(logicalH) * effRatio));

        platformLiveMetrics_.clientBounds = Rect2D{0, 0, logicalW, logicalH};
        platformLiveMetrics_.backBufferWidth = physW;
        platformLiveMetrics_.backBufferHeight = physH;
        platformLiveMetrics_.pixelRatio = effRatio;

        applyCanvasBackingSize(logicalW, logicalH, effRatio);
    }

    /// Sets the canvas backing buffer to `(logical * effRatio)` physical pixels
    /// while leaving the CSS display size unchanged.
    void applyCanvasBackingSize(i32 logicalW, i32 logicalH, f32 effRatio) noexcept
    {
        if (logicalW <= 0 || logicalH <= 0) {
            return;
        }
        const i32 physW = static_cast<i32>(std::lround(static_cast<f32>(logicalW) * effRatio));
        const i32 physH = static_cast<i32>(std::lround(static_cast<f32>(logicalH) * effRatio));
        emscripten_set_canvas_element_size(targetCanvas_.c_str(), physW, physH);
    }

    void applyClientBounds(const Rect2D& clientBoundsIn) noexcept
    {
        // NOTE: clientBoundsIn is in logical pixels. Update the canvas CSS
        // size via JS and the backing buffer via the effective pixel ratio.
        const f32 effRatio = computeEffectivePixelRatio(highDPI_, queryRawPixelRatioFromBrowser());
        platformLiveMetrics_.clientBounds = Rect2D{
            0,
            0,
            clientBoundsIn.width,
            clientBoundsIn.height,
        };
        platformLiveMetrics_.pixelRatio = effRatio;
        platformLiveMetrics_.backBufferWidth = static_cast<i32>(std::lround(static_cast<f32>(clientBoundsIn.width) * effRatio));
        platformLiveMetrics_.backBufferHeight = static_cast<i32>(std::lround(static_cast<f32>(clientBoundsIn.height) * effRatio));

        // NOTE: Update the canvas CSS size to match the requested logical
        // size, then size the backing buffer to physical pixels.
        emscripten_set_element_css_size(
            targetCanvas_.c_str(),
            static_cast<f64>(clientBoundsIn.width),
            static_cast<f64>(clientBoundsIn.height));
        applyCanvasBackingSize(clientBoundsIn.width, clientBoundsIn.height, effRatio);

        if (eventQueue_) {
            // NOTE: Enqueue a surface-update event so the GameHost can resize the
            //       graphics device (OpenGL viewport) on the same frame.
            eventQueue_->enqueue(SystemEvent{
                .kind = SystemEventKind::ViewNeedsUpdateSurfaceEvent,
                .data = {},
            });
        }
    }

    [[nodiscard]] std::unique_ptr<Error>
    applyWindowMode(WindowMode windowMode) noexcept;
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

EM_BOOL
onBrowserResize(int /*eventType*/, const EmscriptenUiEvent* /*event*/, void* userData) noexcept
{
    auto* window = static_cast<GameWindowEmscriptenImpl*>(userData);
    POMDOG_ASSERT(window != nullptr);
    window->onBrowserResized();
    return EM_TRUE;
}

[[nodiscard]] std::unique_ptr<Error>
GameWindowEmscriptenImpl::initialize(
    const std::string& targetCanvas,
    i32 clientWidth,
    i32 clientHeight,
    const HighDPISettings& highDPIIn,
    const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept
{
    eventQueue_ = eventQueue;
    targetCanvas_ = targetCanvas;
    highDPI_ = highDPIIn;

    // NOTE: If the bootstrap supplied an explicit initial size, set the CSS
    // (logical) size of the canvas. The backing buffer is then sized to
    // physical pixels below.
    if (clientWidth > 0 && clientHeight > 0) {
        emscripten_set_element_css_size(
            targetCanvas_.c_str(),
            static_cast<f64>(clientWidth),
            static_cast<f64>(clientHeight));
    }

    // NOTE: Compute the live metrics from the (newly sized) CSS dimensions
    // and the current devicePixelRatio. This also writes the canvas backing
    // buffer at physical pixels; fixing the core HiDPI bug.
    refreshPlatformLiveMetrics();
    committedMetrics_ = platformLiveMetrics_;
    savedCanvasBounds_ = committedMetrics_.clientBounds;

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

    // NOTE: Subscribe to browser resize events so devicePixelRatio changes
    // (window moves across displays, OS scale changes) are also caught.
    emscripten_set_resize_callback(
        EMSCRIPTEN_EVENT_TARGET_WINDOW,
        this,
        EM_FALSE,
        onBrowserResize);

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
GameWindowEmscriptenImpl::applyWindowMode(WindowMode windowMode) noexcept
{
    if (windowMode_ == windowMode) {
        return nullptr;
    }

    // NOTE: Exit current mode first.
    switch (windowMode_) {
    case WindowMode::Fullscreen: {
        emscripten_exit_fullscreen();
        windowModeBeforeFullscreen_ = WindowMode::Windowed;
        // NOTE: Restore canvas size (logical, then backing).
        applyCanvasBackingSize(
            savedCanvasBounds_.width,
            savedCanvasBounds_.height,
            platformLiveMetrics_.pixelRatio);
        break;
    }
    case WindowMode::BrowserSoftFullscreen: {
        emscripten_exit_soft_fullscreen();
        applyCanvasBackingSize(
            savedCanvasBounds_.width,
            savedCanvasBounds_.height,
            platformLiveMetrics_.pixelRatio);
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
        savedCanvasBounds_ = platformLiveMetrics_.clientBounds;

        EmscriptenFullscreenStrategy strategy;
        strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_STRETCH;
        strategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
        strategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
        strategy.canvasResizedCallback = [](int /*eventType*/, const void* /*reserved*/, void* userData) -> EM_BOOL {
            auto* self = static_cast<GameWindowEmscriptenImpl*>(userData);
            // NOTE: Re-read the CSS size that Emscripten just applied and
            // resize the backing store to physical pixels so the canvas stays
            // sharp on HiDPI displays.
            self->refreshPlatformLiveMetrics();
            if (self->eventQueue_) {
                self->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::ViewNeedsUpdateSurfaceEvent,
                    .data = {},
                });
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
        savedCanvasBounds_ = platformLiveMetrics_.clientBounds;
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
            self->refreshPlatformLiveMetrics();
            if (self->eventQueue_) {
                self->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::ViewNeedsUpdateSurfaceEvent,
                    .data = {},
                });
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
    i32 clientWidth,
    i32 clientHeight,
    const HighDPISettings& highDPI,
    const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept
{
    auto window = std::make_shared<GameWindowEmscriptenImpl>();
    if (auto err = window->initialize(targetCanvas, clientWidth, clientHeight, highDPI, eventQueue); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(window), nullptr);
}

} // namespace pomdog::detail::emscripten
