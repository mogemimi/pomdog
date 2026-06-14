// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/win32/game_window_win32.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/application/display_metrics.h"
#include "pomdog/application/high_dpi_settings.h"
#include "pomdog/application/mouse_cursor.h"
#include "pomdog/application/win32/dark_mode.h"
#include "pomdog/application/window_mode.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/input/win32/keyboard_win32.h"
#include "pomdog/input/win32/mouse_win32.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/utfcpp_headers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <dwmapi.h>
#include <objbase.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {
namespace {

template <typename T>
[[nodiscard]] LPTSTR
makeIntegerResource(T&& resource) noexcept
{
#pragma warning(push)
#pragma warning(disable : 4302)
    return MAKEINTRESOURCE(std::forward<T>(resource));
#pragma warning(pop)
}

[[nodiscard]] LPCTSTR
toStandardCursorID(MouseCursor cursor) noexcept
{
    switch (cursor) {
    case MouseCursor::Arrow:
        return IDC_ARROW;
    case MouseCursor::IBeam:
        return IDC_IBEAM;
    case MouseCursor::PointingHand:
        return IDC_HAND;
    case MouseCursor::ResizeHorizontal:
        return IDC_SIZEWE;
    case MouseCursor::ResizeVertical:
        return IDC_SIZENS;
    }
    return IDC_ARROW;
}

[[nodiscard]] std::unique_ptr<Error>
registerInputDevices(HWND windowHandle) noexcept
{
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT)0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT)0x02)
#endif
#ifndef HID_USAGE_GENERIC_KEYBOARD
#define HID_USAGE_GENERIC_KEYBOARD ((USHORT)0x06)
#endif

    std::array<RAWINPUTDEVICE, 2> inputDevices;

    auto& mouse = inputDevices[0];
    mouse.usUsagePage = HID_USAGE_PAGE_GENERIC;
    mouse.usUsage = HID_USAGE_GENERIC_MOUSE;
    mouse.dwFlags = 0;
    mouse.hwndTarget = windowHandle;

    auto& keyboard = inputDevices[1];
    keyboard.usUsagePage = HID_USAGE_PAGE_GENERIC;
    keyboard.usUsage = HID_USAGE_GENERIC_KEYBOARD;
    keyboard.dwFlags = 0;
    keyboard.hwndTarget = windowHandle;

    BOOL success = ::RegisterRawInputDevices(
        inputDevices.data(),
        static_cast<UINT>(inputDevices.size()),
        static_cast<UINT>(sizeof(inputDevices[0])));

    if (success == FALSE) {
        return errors::make("RegisterRawInputDevices() failed");
    }

    return nullptr;
}

[[nodiscard]] UINT
getDpiForWindowOrSystem(HWND windowHandle) noexcept
{
    if (windowHandle != nullptr) {
        if (const UINT dpi = ::GetDpiForWindow(windowHandle); dpi != 0) {
            return dpi;
        }
    }
    return ::GetDpiForSystem();
}

void enterBorderlessFullscreen(HWND windowHandle) noexcept
{
    // NOTE: Get monitor info.
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(MONITORINFO);
    ::GetMonitorInfo(::MonitorFromWindow(windowHandle, MONITOR_DEFAULTTONEAREST), &monitorInfo);

    const auto& monitorRect = monitorInfo.rcMonitor;
    const auto width = monitorRect.right - monitorRect.left;
    const auto height = monitorRect.bottom - monitorRect.top;

    const DWORD windowStyle = WS_POPUP | WS_VISIBLE;
    ::SetWindowLongPtr(windowHandle, GWL_STYLE, windowStyle);
    ::SetWindowPos(
        windowHandle,
        HWND_TOP,
        monitorRect.left,
        monitorRect.top,
        width,
        height,
        SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
}

/// Switches the window to the framed windowed style and resizes it so that
/// the client area matches physicalClientSize (width/height in physical
/// pixels). The frame is computed for the window's current DPI. When position
/// has a value, the outer rect's top-left corner is moved there; otherwise
/// the window keeps its current on-screen position.
void enterWindowedMode(
    HWND windowHandle,
    const std::optional<::POINT>& position,
    const Rect2D& physicalClientSize,
    bool allowUserResizing) noexcept
{
    DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX;
    if (allowUserResizing) {
        // NOTE: `WS_THICKFRAME` enables the resize border; `WS_MAXIMIZEBOX` enables the maximize
        //       button in the title bar so users can maximize the window from the UI.
        windowStyle |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }
    windowStyle |= WS_VISIBLE;

    RECT windowRect = {0, 0, physicalClientSize.width, physicalClientSize.height};
    ::AdjustWindowRectExForDpi(&windowRect, windowStyle, FALSE, 0, getDpiForWindowOrSystem(windowHandle));
    const int adjustedWidth = static_cast<int>(windowRect.right - windowRect.left);
    const int adjustedHeight = static_cast<int>(windowRect.bottom - windowRect.top);

    ::SetWindowLongPtr(windowHandle, GWL_STYLE, windowStyle);

    // NOTE: Always pass `SWP_SHOWWINDOW` to ensure the window becomes visible after a style
    // change (e.g. `WS_POPUP` -> `WS_OVERLAPPED`). Without a target position, pass `SWP_NOMOVE`
    // so the window keeps its current on-screen position.
    UINT posFlags = SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_SHOWWINDOW;
    if (!position.has_value()) {
        posFlags |= SWP_NOMOVE;
    }
    ::SetWindowPos(
        windowHandle,
        HWND_NOTOPMOST,
        position.has_value() ? position->x : 0,
        position.has_value() ? position->y : 0,
        adjustedWidth,
        adjustedHeight,
        posFlags);
}

/// Switches the window to the borderless (WS_POPUP) style and resizes it so
/// that the client area matches physicalClientSize (width/height in physical
/// pixels). When allowUserResizing is true the style carries WS_THICKFRAME,
/// whose invisible resize border lies outside the client area. When position
/// has a value, the outer rect's top-left corner is moved there; otherwise
/// the window keeps its current on-screen position.
void enterBorderlessWindowedMode(
    HWND windowHandle,
    const std::optional<::POINT>& position,
    const Rect2D& physicalClientSize,
    bool allowUserResizing) noexcept
{
    DWORD windowStyle = WS_POPUP;
    if (allowUserResizing) {
        windowStyle |= WS_THICKFRAME;
    }
    windowStyle |= WS_VISIBLE;

    // NOTE: `WS_POPUP | WS_THICKFRAME` has an invisible resize frame around the window.
    // `AdjustWindowRectExForDpi` accounts for it so `SetWindowPos` yields the requested
    // client size. For plain `WS_POPUP` the adjustment is an identity.
    RECT windowRect = {0, 0, physicalClientSize.width, physicalClientSize.height};
    ::AdjustWindowRectExForDpi(&windowRect, windowStyle, FALSE, 0, getDpiForWindowOrSystem(windowHandle));
    const int adjustedWidth = static_cast<int>(windowRect.right - windowRect.left);
    const int adjustedHeight = static_cast<int>(windowRect.bottom - windowRect.top);

    ::SetWindowLongPtr(windowHandle, GWL_STYLE, windowStyle);

    // NOTE: Without a target position, pass `SWP_NOMOVE` so the window keeps its current
    // on-screen position.
    UINT posFlags = SWP_FRAMECHANGED | SWP_NOOWNERZORDER;
    if (!position.has_value()) {
        posFlags |= SWP_NOMOVE;
    }
    ::SetWindowPos(
        windowHandle,
        HWND_NOTOPMOST,
        position.has_value() ? position->x : 0,
        position.has_value() ? position->y : 0,
        adjustedWidth,
        adjustedHeight,
        posFlags);

    // NOTE: Disable DWM rounded corners so the window is truly borderless.
    // `DWMWA_WINDOW_CORNER_PREFERENCE` (attribute 33) is a Windows 11+ API introduced in
    // build 22000. On Windows 10, `DwmSetWindowAttribute()` silently returns `E_INVALIDARG`
    // but does NOT crash. The return value is intentionally ignored here because the fallback
    // behaviour on Windows 10 (slightly rounded DWM corners) is acceptable.
    const DWORD cornerPreference = DWMWCP_DONOTROUND;
    ::DwmSetWindowAttribute(windowHandle, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference, sizeof(cornerPreference));
}

class GameWindowWin32Impl final : public GameWindowWin32 {
private:
    std::shared_ptr<SystemEventQueue> eventQueue_;
    std::string title_;

    // NOTE: The client-area size requested at window creation, in physical
    // pixels at the creation-time system DPI. Immutable after initialize();
    // it is used only when no live geometry is available: the startup
    // BorderlessWindowed mode and the exit-Fullscreen path when no
    // pre-fullscreen placement could be captured.
    Rect2D initialPhysicalClientBounds_;
    std::optional<HCURSOR> gameCursor_;

    // NOTE: Placement captured immediately before entering Fullscreen, used
    // to restore the window when leaving Fullscreen. The client size is
    // stored instead of the outer rect so that the restore path can
    // recompute the frame for its own style and DPI; the captured mode may
    // have been borderless, whose outer rect carries no frame.
    struct PreFullscreenPlacement final {
        ::POINT outerTopLeft;
        i32 physicalClientWidth;
        i32 physicalClientHeight;
    };
    std::optional<PreFullscreenPlacement> preFullscreenPlacement_;
    HINSTANCE instanceHandle_ = nullptr;
    HWND windowHandle_ = nullptr;
    WindowMode windowMode_ = WindowMode::Windowed;
    bool allowUserResizing_ = false;
    bool isMouseCursorVisible_ = true;

    // NOTE: Set while the window is minimized. Used to force a metrics commit
    // when leaving the minimized state; see the WM_SIZE handler.
    bool wasMinimized_ = false;

    HighDPISettings highDPI_ = {};

    // NOTE: `committedMetrics_` is the snapshot exposed via getClientBounds(),
    // getPixelRatio() and getDisplayMetrics(). It is updated only at frame
    // boundary via commitDisplayMetricsIfChanged().
    // `platformLiveMetrics_` mirrors the latest values reported by the OS
    // (WM_SIZE / WM_DPICHANGED) and may differ from committedMetrics_ until
    // the next frame boundary.
    DisplayMetrics committedMetrics_ = {};
    DisplayMetrics platformLiveMetrics_ = {};

    // NOTE: Pending requests - set by `request...()` and applied at the next
    //       `applyPendingWindowRequests()` call at the frame boundary.
    std::optional<WindowMode> pendingWindowMode_;
    std::optional<Rect2D> pendingClientBounds_;
    std::optional<std::string> pendingTitle_;
    std::optional<bool> pendingAllowUserResizing_;

public:
    ~GameWindowWin32Impl()
    {
        if (windowHandle_ != nullptr) {
            ::SetWindowLongPtr(windowHandle_, GWLP_USERDATA, 0);
            ::DestroyWindow(windowHandle_);
            windowHandle_ = nullptr;

            ::CoUninitialize();
        }

        instanceHandle_ = nullptr;
    }

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        HINSTANCE hInstance,
        int nCmdShow,
        HICON icon,
        HICON iconSmall,
        bool useOpenGL,
        const std::shared_ptr<SystemEventQueue>& eventQueueIn,
        const gpu::PresentationParameters& presentationParameters,
        const HighDPISettings& highDPIIn) noexcept
    {
        eventQueue_ = eventQueueIn;
        title_ = "Game";
        highDPI_ = highDPIIn;

        // NOTE: `presentationParameters.backBufferWidth/Height` are in physical
        // pixels. The bootstrap precomputes them from the user-requested
        // logical client size and the system pixel ratio. We use these for
        // CreateWindowEx (PMv2 expects physical pixels when DPI-aware).
        const i32 initialPhysWidth = presentationParameters.backBufferWidth;
        const i32 initialPhysHeight = presentationParameters.backBufferHeight;
        initialPhysicalClientBounds_ = Rect2D{0, 0, initialPhysWidth, initialPhysHeight};
        instanceHandle_ = hInstance;
        windowHandle_ = nullptr;
        allowUserResizing_ = false;
        windowMode_ = presentationParameters.windowMode;
        isMouseCursorVisible_ = true;

        if (windowMode_ == WindowMode::BrowserSoftFullscreen) {
            return errors::make("BrowserSoftFullscreen mode is not supported on Windows.");
        }

        POMDOG_ASSERT(initialPhysWidth > 0);
        POMDOG_ASSERT(initialPhysHeight > 0);

        DWORD windowStyle = 0;
        DWORD windowStyleEx = 0;
        LONG adjustedWidth = static_cast<LONG>(initialPhysWidth);
        LONG adjustedHeight = static_cast<LONG>(initialPhysHeight);

        if (useOpenGL) {
            windowStyle |= WS_CLIPCHILDREN;
            windowStyle |= WS_CLIPSIBLINGS;
        }

        if (windowMode_ == WindowMode::Fullscreen || windowMode_ == WindowMode::BorderlessWindowed) {
            windowStyle |= WS_POPUP;
            if (windowMode_ == WindowMode::Fullscreen) {
                windowStyleEx |= WS_EX_TOPMOST;
            }
        }
        else {
            constexpr DWORD fixedWindowStyle = (WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
            windowStyle |= fixedWindowStyle;
            // NOTE: `WS_THICKFRAME` and `WS_MAXIMIZEBOX` are added later via
            //       `requestAllowUserResizing()` before the first frame renders.

            // NOTE: The window does not exist yet, so use the system DPI as an
            // estimate. After creation, the per-monitor DPI is queried via
            // GetDpiForWindow and the precise metrics are committed at the first
            // frame boundary.
            RECT windowRect = {0, 0, static_cast<LONG>(initialPhysWidth), static_cast<LONG>(initialPhysHeight)};
            ::AdjustWindowRectExForDpi(&windowRect, windowStyle, FALSE, 0, ::GetDpiForSystem());

            adjustedWidth = windowRect.right - windowRect.left;
            adjustedHeight = windowRect.bottom - windowRect.top;
        }

        constexpr auto windowName = "Pomdog.GameWindowWin32";

        UINT windowClassStyle = (CS_HREDRAW | CS_VREDRAW);

        if (useOpenGL) {
            windowClassStyle |= CS_OWNDC;
        }

        if (icon == nullptr) {
            icon = ::LoadIcon(instanceHandle_, makeIntegerResource(IDI_APPLICATION));
        }

        if (iconSmall == nullptr) {
            iconSmall = ::LoadIcon(instanceHandle_, makeIntegerResource(IDI_APPLICATION));
        }

        WNDCLASSEX wcex = {
            sizeof(WNDCLASSEX),
            windowClassStyle,
            GameWindowWin32Impl::windowProcedure,
            0,
            0,
            instanceHandle_,
            icon,
            ::LoadCursor(nullptr, IDC_ARROW),
            static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH)),
            nullptr,
            windowName,
            iconSmall,
        };

        if (::RegisterClassEx(&wcex) == 0) {
            return errors::make("RegisterClassEx() failed");
        }

        windowHandle_ = CreateWindowEx(
            windowStyleEx,
            wcex.lpszClassName,
            title_.data(),
            windowStyle,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            adjustedWidth,
            adjustedHeight,
            nullptr,
            nullptr,
            instanceHandle_,
            nullptr);

        if (windowHandle_ == nullptr) {
            return errors::make("CreateWindowEx() failed");
        }

        if (isDarkMode()) {
            if (auto err = useImmersiveDarkMode(windowHandle_, true); err != nullptr) {
                return errors::wrap(std::move(err), "useImmersiveDarkMode() failed");
            }
        }

        // NOTE: See http://msdn.microsoft.com/ja-jp/library/ff485844(v=vs.85).aspx
        if (auto hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE); FAILED(hr)) {
            return errors::make("CoInitializeEx() failed");
        }

        if (nCmdShow == SW_MAXIMIZE) {
            ::ShowWindow(windowHandle_, SW_RESTORE);
        }
        else {
            ::ShowWindow(windowHandle_, nCmdShow);
        }

        // NOTE: Initialize the committed and live display metrics from the
        // freshly created window. After this point all display state is read
        // from `committedMetrics_`.
        platformLiveMetrics_ = computePlatformLiveMetricsFromHwnd();
        committedMetrics_ = platformLiveMetrics_;

        ::SetWindowLongPtr(windowHandle_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        if (auto err = registerInputDevices(windowHandle_); err != nullptr) {
            return errors::wrap(std::move(err), "registerInputDevices() failed");
        }

        // NOTE: Apply the initial window mode.
        switch (windowMode_) {
        case WindowMode::Fullscreen: {
            // NOTE: The freshly created window has no frame yet (WS_POPUP), so the
            // captured client size equals the requested initial client size.
            preFullscreenPlacement_ = capturePreFullscreenPlacement();
            enterBorderlessFullscreen(windowHandle_);
            break;
        }
        case WindowMode::Maximized: {
            ::ShowWindow(windowHandle_, SW_MAXIMIZE);
            break;
        }
        case WindowMode::BorderlessWindowed: {
            enterBorderlessWindowedMode(windowHandle_, std::nullopt, initialPhysicalClientBounds_, allowUserResizing_);
            break;
        }
        case WindowMode::Windowed:
        case WindowMode::BrowserSoftFullscreen:
            break;
        }

        return nullptr;
    }

    bool
    getAllowUserResizing() const override
    {
        return allowUserResizing_;
    }

    void
    requestAllowUserResizing(bool allowResizingIn) override
    {
        pendingAllowUserResizing_ = allowResizingIn;
    }

    std::string
    getTitle() const override
    {
        return title_;
    }

    void
    requestTitle(const std::string& titleIn) override
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
    commitDisplayMetricsIfChanged(std::optional<f32> pixelRatioHint) noexcept override
    {
        // NOTE: Snapshot the latest OS values; computePlatformLive may not
        // have caught every change (initial creation, programmatic SetWindowPos
        // may dispatch WM_SIZE synchronously into our state but not always).
        // `pixelRatioHint` carries the ratio reported by WM_DPICHANGED;
        // when present it is trusted instead of re-querying GetDpiForWindow,
        // which can briefly lag behind the message.
        platformLiveMetrics_ = computePlatformLiveMetricsFromHwnd(pixelRatioHint);
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

    void
    setMouseCursorVisible(bool visibleIn) override
    {
        isMouseCursorVisible_ = visibleIn;

        if (isMouseCursorVisible_) {
            if (gameCursor_) {
                ::SetCursor(*gameCursor_);
            }
            else {
                auto nativeCursor = ::LoadCursor(nullptr, toStandardCursorID(MouseCursor::Arrow));
                ::SetCursor(nativeCursor);
            }
        }
        else {
            ::SetCursor(nullptr);
        }
    }

    void
    setMouseCursor(MouseCursor cursorIn) override
    {
        gameCursor_ = ::LoadCursor(nullptr, toStandardCursorID(cursorIn));

        if (isMouseCursorVisible_) {
            ::SetCursor(*gameCursor_);
        }
    }

    bool
    isMinimized() const override
    {
        return (::IsIconic(windowHandle_) == TRUE);
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
        if (windowMode == WindowMode::BrowserSoftFullscreen) {
            return errors::make("BrowserSoftFullscreen mode is not supported on Windows.");
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
        // NOTE: Apply window-mode first, so that subsequent clientBounds or
        //       resizing requests observe the new mode.
        if (pendingWindowMode_.has_value()) {
            applyWindowMode(*pendingWindowMode_);
            pendingWindowMode_ = std::nullopt;
        }

        // NOTE: Discard pending clientBounds when the committed mode does not
        //       support arbitrary client sizes.
        if (pendingClientBounds_.has_value()) {
            if (windowMode_ != WindowMode::Fullscreen && windowMode_ != WindowMode::Maximized) {
                applyClientBounds(*pendingClientBounds_);
            }
            pendingClientBounds_ = std::nullopt;
        }

        if (pendingTitle_.has_value()) {
            applyTitle(*pendingTitle_);
            pendingTitle_ = std::nullopt;
        }

        if (pendingAllowUserResizing_.has_value()) {
            applyAllowUserResizing(*pendingAllowUserResizing_);
            pendingAllowUserResizing_ = std::nullopt;
        }
    }

    void
    close() override
    {
        ::CloseWindow(windowHandle_);
    }

    HWND
    getNativeWindowHandle() const override
    {
        return windowHandle_;
    }

private:
    [[nodiscard]] f32
    queryRawPixelRatio() const noexcept
    {
        if (windowHandle_ == nullptr) {
            return 1.0f;
        }
        const UINT dpi = ::GetDpiForWindow(windowHandle_);
        if (dpi == 0) {
            return 1.0f;
        }
        return static_cast<f32>(dpi) / 96.0f;
    }

    [[nodiscard]] DisplayMetrics
    computePlatformLiveMetricsFromHwnd(std::optional<f32> pixelRatioHint = std::nullopt) const noexcept
    {
        DisplayMetrics metrics = {};
        if (windowHandle_ == nullptr) {
            return metrics;
        }

        // NOTE: Desktop reports the platform pixel ratio without applying
        // `maxPixelRatio`. The OS sizes the window backing store at its own
        // effective DPI, so reporting a clamped ratio here would make
        // `pixelRatio` inconsistent with the actual back-buffer size.
        //
        // NOTE: When a fresh ratio is supplied by WM_DPICHANGED
        // (`pixelRatioHint`), trust it instead of re-querying GetDpiForWindow,
        // which can briefly lag behind the message.
        const f32 ratio = pixelRatioHint.has_value()
                              ? *pixelRatioHint
                              : computeUnclampedPixelRatio(highDPI_, queryRawPixelRatio());
        metrics.pixelRatio = ratio;

        ::RECT clientRect = {};
        ::GetClientRect(windowHandle_, &clientRect);

        // NOTE: Clamp the reported size to 1x1 physical pixels. The OS can
        // report a zero-sized client area (e.g. while the window is
        // minimized), but the graphics backends require positive back-buffer
        // dimensions (GraphicsContextDirect3D11::resizeBackBuffers() asserts
        // on zero) and game code divides by the client size for aspect
        // ratios. The window shows no content in those states, so the
        // 1-pixel metrics are never visible on screen.
        const i32 physWidth = std::max(static_cast<i32>(clientRect.right - clientRect.left), 1);
        const i32 physHeight = std::max(static_cast<i32>(clientRect.bottom - clientRect.top), 1);

        metrics.backBufferWidth = physWidth;
        metrics.backBufferHeight = physHeight;

        // NOTE: Convert physical -> logical using the same ratio so the
        // logical client size matches what the OS reports for input coordinates.
        // Clamped to 1x1 for the same reason as the physical size above (the
        // rounding can yield zero for a 1-pixel client at pixel ratios above 2).
        const f32 inv = (ratio > 0.0f) ? (1.0f / ratio) : 1.0f;
        metrics.clientBounds.width = std::max(static_cast<i32>(std::lround(static_cast<f32>(physWidth) * inv)), 1);
        metrics.clientBounds.height = std::max(static_cast<i32>(std::lround(static_cast<f32>(physHeight) * inv)), 1);

        ::POINT clientOrigin = {0, 0};
        if (::ClientToScreen(windowHandle_, &clientOrigin) != 0) {
            metrics.clientBounds.x = static_cast<i32>(std::lround(static_cast<f32>(clientOrigin.x) * inv));
            metrics.clientBounds.y = static_cast<i32>(std::lround(static_cast<f32>(clientOrigin.y) * inv));
        }
        return metrics;
    }

    /// Returns the window's current outer position and physical client size,
    /// or std::nullopt when the window geometry cannot be queried.
    [[nodiscard]] std::optional<PreFullscreenPlacement>
    capturePreFullscreenPlacement() const noexcept
    {
        ::RECT outerRect = {};
        ::RECT clientRect = {};
        if ((::GetWindowRect(windowHandle_, &outerRect) == FALSE) ||
            (::GetClientRect(windowHandle_, &clientRect) == FALSE)) {
            return std::nullopt;
        }
        return PreFullscreenPlacement{
            .outerTopLeft = {outerRect.left, outerRect.top},
            .physicalClientWidth = static_cast<i32>(clientRect.right - clientRect.left),
            .physicalClientHeight = static_cast<i32>(clientRect.bottom - clientRect.top),
        };
    }

    void applyWindowMode(WindowMode windowMode) noexcept
    {
        POMDOG_ASSERT(windowHandle_ != nullptr);

        if (windowMode == windowMode_) {
            return;
        }

        // NOTE: Exit current mode before transitioning.
        if (windowMode_ == WindowMode::Fullscreen) {
            if (preFullscreenPlacement_.has_value()) {
                const auto clientSize = Rect2D{
                    0,
                    0,
                    preFullscreenPlacement_->physicalClientWidth,
                    preFullscreenPlacement_->physicalClientHeight,
                };
                enterWindowedMode(windowHandle_, preFullscreenPlacement_->outerTopLeft, clientSize, allowUserResizing_);
            }
            else {
                // NOTE: No placement was captured (the query failed at fullscreen
                // entry); fall back to the initial client size at the current position.
                enterWindowedMode(windowHandle_, std::nullopt, initialPhysicalClientBounds_, allowUserResizing_);
            }
            windowMode_ = WindowMode::Windowed;
            preFullscreenPlacement_ = std::nullopt;
        }
        else if (windowMode_ == WindowMode::Maximized) {
            ::ShowWindow(windowHandle_, SW_RESTORE);
            windowMode_ = WindowMode::Windowed;
        }
        else if (windowMode_ == WindowMode::BorderlessWindowed) {
            // NOTE: Restore windowed style and re-enable DWM rounded corners.
            // `DWMWA_WINDOW_CORNER_PREFERENCE` is Windows 11+ only; on Windows 10 the call
            // silently fails with `E_INVALIDARG` (no crash). See `enterBorderlessWindowedMode()`.
            //
            // NOTE: Keep the current client-area size across the style change so the visible
            // canvas does not change. `platformLiveMetrics_` is used instead of
            // `committedMetrics_` because it already reflects resizes processed earlier in
            // this frame; the commit happens only after the pending window requests are applied.
            const auto currentPhysicalClientSize = Rect2D{
                0,
                0,
                platformLiveMetrics_.backBufferWidth,
                platformLiveMetrics_.backBufferHeight,
            };
            enterWindowedMode(windowHandle_, std::nullopt, currentPhysicalClientSize, allowUserResizing_);
            const DWORD cornerPreference = DWMWCP_DEFAULT;
            ::DwmSetWindowAttribute(windowHandle_, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference, sizeof(cornerPreference));
            windowMode_ = WindowMode::Windowed;
        }

        if (windowMode == windowMode_) {
            enqueueWindowModeChanged(windowMode_);
            return;
        }

        switch (windowMode) {
        case WindowMode::Windowed: {
            // NOTE: Already windowed (fell through from fullscreen/maximized exit above).
            break;
        }
        case WindowMode::BorderlessWindowed: {
            // NOTE: Keep the current client-area size across the style change; the outer
            // top-left corner stays where the previous window was. `platformLiveMetrics_`
            // already reflects the synchronous WM_SIZE dispatched by the mode-exit
            // transition above (e.g. leaving Fullscreen or Maximized), while
            // `committedMetrics_` would still hold the pre-transition size.
            std::optional<::POINT> position;
            if (::RECT outerRect = {}; ::GetWindowRect(windowHandle_, &outerRect) == TRUE) {
                position = ::POINT{outerRect.left, outerRect.top};
            }
            const auto currentPhysicalClientSize = Rect2D{
                0,
                0,
                platformLiveMetrics_.backBufferWidth,
                platformLiveMetrics_.backBufferHeight,
            };
            enterBorderlessWindowedMode(windowHandle_, position, currentPhysicalClientSize, allowUserResizing_);
            break;
        }
        case WindowMode::Maximized: {
            ::ShowWindow(windowHandle_, SW_MAXIMIZE);
            break;
        }
        case WindowMode::Fullscreen: {
            preFullscreenPlacement_ = capturePreFullscreenPlacement();
            enterBorderlessFullscreen(windowHandle_);
            break;
        }
        case WindowMode::BrowserSoftFullscreen:
            // NOTE: Validated and rejected in `requestWindowMode()`; should not reach here.
            break;
        }

        windowMode_ = windowMode;
        enqueueWindowModeChanged(windowMode_);
    }

    void applyClientBounds(const Rect2D& clientBoundsIn) noexcept
    {
        POMDOG_ASSERT(windowHandle_ != nullptr);

        DWORD const dwStyle = static_cast<DWORD>(::GetWindowLong(windowHandle_, GWL_STYLE));

        // NOTE: clientBoundsIn is in logical pixels. Convert to physical using
        // the OS's raw DPI so that the window's client area matches the
        // requested logical size visually.
        const f32 rawRatio = queryRawPixelRatio();
        const i32 physClientWidth = static_cast<i32>(std::lround(static_cast<f32>(clientBoundsIn.width) * rawRatio));
        const i32 physClientHeight = static_cast<i32>(std::lround(static_cast<f32>(clientBoundsIn.height) * rawRatio));

        int adjustedWidth = physClientWidth;
        int adjustedHeight = physClientHeight;

        if (windowMode_ != WindowMode::BorderlessWindowed) {
            // NOTE: For windowed modes with a frame, adjust for the window chrome
            // using the window's current per-monitor DPI.
            RECT windowRect = {0, 0, physClientWidth, physClientHeight};
            ::AdjustWindowRectExForDpi(&windowRect, dwStyle, FALSE, 0, getDpiForWindowOrSystem(windowHandle_));
            adjustedWidth = static_cast<int>(windowRect.right - windowRect.left);
            adjustedHeight = static_cast<int>(windowRect.bottom - windowRect.top);
        }

        constexpr UINT flags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE;

        if (0 == ::SetWindowPos(windowHandle_, 0, 0, 0, adjustedWidth, adjustedHeight, flags)) {
            return;
        }

        // NOTE: WM_SIZE has updated platformLiveMetrics_ synchronously. The
        // following events will trigger commitDisplayMetricsIfChanged at the
        // next frame boundary in GameHostWin32::doEvents().
        eventQueue_->enqueue(SystemEvent{
            .kind = SystemEventKind::ViewWillStartLiveResizeEvent,
            .data = {},
        });
        eventQueue_->enqueue(SystemEvent{
            .kind = SystemEventKind::ViewDidEndLiveResizeEvent,
            .data = {},
        });
    }

    void applyTitle(const std::string& titleIn) noexcept
    {
        POMDOG_ASSERT(windowHandle_ != nullptr);

        // NOTE: Convert UTF-8 to UTF-16 (WCHAR) for the Windows Unicode API.
        // `SetWindowTextW` expects LPCWSTR; passing a UTF-8 `const char*` directly would
        // corrupt multi-byte characters such as CJK text.
        const int wideLen = ::MultiByteToWideChar(CP_UTF8, 0, titleIn.c_str(), -1, nullptr, 0);
        if (wideLen <= 0) {
            return;
        }
        std::wstring wideTitle(static_cast<std::size_t>(wideLen - 1), L'\0');
        if (::MultiByteToWideChar(CP_UTF8, 0, titleIn.c_str(), -1, wideTitle.data(), wideLen) == 0) {
            return;
        }

        if (::SetWindowTextW(windowHandle_, wideTitle.c_str()) == 0) {
            return;
        }

        title_ = titleIn;
    }

    void applyAllowUserResizing(bool allowResizingIn) noexcept
    {
        POMDOG_ASSERT(windowHandle_ != nullptr);

        if (windowMode_ == WindowMode::Fullscreen) {
            return;
        }
        // NOTE: `BorderlessWindowed` mode uses `WS_POPUP`, which can carry `WS_THICKFRAME`.
        // Allow the toggle here so the resize handle is updated even in borderless mode.

        LONG_PTR windowStyle = ::GetWindowLongPtr(windowHandle_, GWL_STYLE);
        if (0 == windowStyle) {
            return;
        }

        if (allowResizingIn) {
            windowStyle |= WS_THICKFRAME | WS_MAXIMIZEBOX;
        }
        else {
            windowStyle &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
        }

        if (0 == ::SetWindowLongPtr(windowHandle_, GWL_STYLE, windowStyle)) {
            return;
        }

        if (::SetWindowPos(
                windowHandle_, 0, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_FRAMECHANGED) == 0) {
            return;
        }

        allowUserResizing_ = allowResizingIn;
    }

    void enqueueWindowModeChanged(WindowMode newMode) noexcept
    {
        if (eventQueue_) {
            eventQueue_->enqueue(SystemEvent{
                .kind = SystemEventKind::WindowModeChangedEvent,
                .data = WindowModeChangedEvent{.windowMode = newMode},
            });
        }
    }

    static LRESULT CALLBACK
    windowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        auto window = reinterpret_cast<GameWindowWin32Impl*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

        POMDOG_ASSERT(WM_QUIT != msg);

        switch (msg) {
        case WM_CLOSE: {
            if (window) {
                window->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::WindowShouldCloseEvent,
                    .data = {},
                });
            }
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            [[maybe_unused]] HDC hdc = ::BeginPaint(hWnd, &ps);
            ::EndPaint(hWnd, &ps);
            break;
        }
        case WM_KEYDOWN: {
            return 0;
        }
        case WM_UNICHAR:
            [[fallthrough]];
        case WM_SYSCHAR:
            [[fallthrough]];
        case WM_CHAR: {
            if ((msg == WM_UNICHAR) && (wParam == UNICODE_NOCHAR)) {
                // TODO: Not implemented
                return TRUE;
            }

            if (window) {
                std::string text;
                if (msg == WM_UNICHAR) {
                    // NOTE: `WM_UNICHAR` provides a UTF-32 code point.
                    const auto codePoint = static_cast<char32_t>(wParam);
                    utf8::append(codePoint, std::back_inserter(text));
                }
                else {
                    // NOTE: `WM_CHAR`/`WM_SYSCHAR` provides a UTF-16 code unit.
                    const auto ch = static_cast<wchar_t>(wParam);
                    std::wstring wstr(1, ch);
                    utf8::utf16to8(wstr.begin(), wstr.end(), std::back_inserter(text));
                }
                if (!text.empty()) {
                    window->eventQueue_->enqueue(SystemEvent{
                        .kind = SystemEventKind::InputTextEvent,
                        .data = InputTextEvent{
                            .text = std::move(text),
                        },
                    });
                }
            }
            return 0;
        }
        case WM_ENTERSIZEMOVE: {
            if (window) {
                window->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::ViewWillStartLiveResizeEvent,
                    .data = {},
                });
            }
            return 0;
        }
        case WM_EXITSIZEMOVE: {
            if (window) {
                window->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::ViewDidEndLiveResizeEvent,
                    .data = {},
                });
            }
            return 0;
        }
        case WM_GETMINMAXINFO: {
            // NOTE: Enforce a minimum client-area size of 1x1 physical pixels so
            // that neither interactive resizing nor programmatic SetWindowPos can
            // shrink the client area to zero; the graphics backends require
            // positive back-buffer dimensions. Without this, dragging the bottom
            // edge up to the title bar yields a zero-height client area (the OS
            // default minimum tracking height only accounts for the frame).
            // This message also arrives during CreateWindowEx, before
            // GWLP_USERDATA is set, so it must not rely on `window`.
            auto* info = reinterpret_cast<MINMAXINFO*>(lParam);
            if (info != nullptr) {
                const auto style = static_cast<DWORD>(::GetWindowLongPtr(hWnd, GWL_STYLE));
                RECT rect = {0, 0, 1, 1};
                ::AdjustWindowRectExForDpi(&rect, style, FALSE, 0, getDpiForWindowOrSystem(hWnd));
                info->ptMinTrackSize.x = std::max(info->ptMinTrackSize.x, rect.right - rect.left);
                info->ptMinTrackSize.y = std::max(info->ptMinTrackSize.y, rect.bottom - rect.top);
            }
            return 0;
        }
        case WM_SIZING: {
            if (window) {
                window->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::ViewNeedsUpdateSurfaceEvent,
                    .data = {},
                });
            }
            return TRUE;
        }
        case WM_SIZE: {
            if (window) {
                // NOTE: lParam carries the new client area size in physical
                // pixels (PMv2 is DPI-aware). Refresh the live metrics from
                // the OS so doEvents() observes the change at the next frame
                // boundary.
                window->platformLiveMetrics_ = window->computePlatformLiveMetricsFromHwnd();

                // NOTE: While minimized, the OS reports a zero-sized client area
                // (clamped to 1x1 by computePlatformLiveMetricsFromHwnd). A metrics
                // commit triggered in that state (e.g. by a pending window request)
                // would stick until the next unrelated resize or DPI event, so
                // force a re-commit when leaving the minimized state.
                // commitDisplayMetricsIfChanged() turns this into a no-op when
                // nothing actually changed.
                if (wParam == SIZE_MINIMIZED) {
                    window->wasMinimized_ = true;
                }
                else if (window->wasMinimized_) {
                    window->wasMinimized_ = false;
                    window->eventQueue_->enqueue(SystemEvent{
                        .kind = SystemEventKind::ViewDidEndLiveResizeEvent,
                        .data = {},
                    });
                }

                // NOTE: Detect user-triggered maximize/restore.
                if (wParam == SIZE_MAXIMIZED) {
                    if (window->windowMode_ != WindowMode::Maximized &&
                        window->windowMode_ != WindowMode::Fullscreen) {
                        window->windowMode_ = WindowMode::Maximized;
                        window->enqueueWindowModeChanged(WindowMode::Maximized);
                    }
                }
                else if (wParam == SIZE_RESTORED) {
                    if (window->windowMode_ == WindowMode::Maximized) {
                        window->windowMode_ = WindowMode::Windowed;
                        window->enqueueWindowModeChanged(WindowMode::Windowed);
                    }
                }
            }
            break;
        }
        case WM_DPICHANGED: {
            if (window) {
                const auto newDpi = HIWORD(wParam);
                const auto* suggestedRect = reinterpret_cast<const RECT*>(lParam);
                if (suggestedRect != nullptr) {
                    // NOTE: Microsoft recommends adopting the proposed window
                    // rectangle when `WM_DPICHANGED` arrives so the window scales
                    // smoothly across monitors of different DPI.
                    // See https://learn.microsoft.com/en-us/windows/win32/hidpi/wm-dpichanged for details.
                    ::SetWindowPos(
                        hWnd,
                        nullptr,
                        suggestedRect->left,
                        suggestedRect->top,
                        suggestedRect->right - suggestedRect->left,
                        suggestedRect->bottom - suggestedRect->top,
                        SWP_NOZORDER | SWP_NOACTIVATE);
                }

                // NOTE: Carry the new ratio (derived from wParam) in the event.
                // doEvents() trusts this value at the next frame boundary and
                // commits the metrics without re-querying GetDpiForWindow, which
                // can lag behind `WM_DPICHANGED`. `maxPixelRatio` is not applied
                // on desktop.
                const f32 raw = static_cast<f32>(newDpi) / 96.0f;
                window->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::PixelRatioChangedEvent,
                    .data = PixelRatioChangedEvent{
                        .newPixelRatio = computeUnclampedPixelRatio(window->highDPI_, raw),
                    },
                });
            }
            return 0;
        }
        case WM_MOVE: {
            if (window) {
                // NOTE: `WM_MOVE` only updates the position. We still refresh
                // the full live metrics so the origin in clientBounds reflects
                // the latest screen-space client-area position.
                window->platformLiveMetrics_ = window->computePlatformLiveMetricsFromHwnd();
            }
            break;
        }
        case WM_ACTIVATE: {
            if (window != nullptr) {
                // NOTE: While the window is inactive it no longer receives raw
                // mouse or keyboard input (the devices are registered without
                // RIDEV_INPUTSINK), so a button or key released elsewhere is
                // never observed and would otherwise stay stuck down until the
                // next matching event. Notify the host to clear the input state
                // on deactivation. See GameHostWin32::processSystemEvents.
                if (LOWORD(wParam) == WA_INACTIVE) {
                    window->eventQueue_->enqueue(SystemEvent{
                        .kind = SystemEventKind::WindowFocusLostEvent,
                        .data = {},
                    });
                }
            }
            break;
        }
        case WM_DESTROY: {
            const int exitCode = 0;
            ::PostQuitMessage(exitCode);
            if (window) {
                window->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::WindowWillCloseEvent,
                    .data = {},
                });
            }
            return 0;
        }
        case WM_SETCURSOR: {
            if (window) {
                const auto hitTest = lParam & 0xffff;
                if (hitTest == HTCLIENT) {
                    if (!window->isMouseCursorVisible_) {
                        ::SetCursor(nullptr);
                        return FALSE;
                    }
                    if (window->gameCursor_) {
                        ::SetCursor(*window->gameCursor_);
                        return FALSE;
                    }
                }
            }
            break;
        }
        case WM_INPUT: {
            if (window) {
                RAWINPUT raw = {};
                UINT size = sizeof(raw);

                ::GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));

                if (raw.header.dwType == RIM_TYPEMOUSE) {
                    translateMouseEvent(window->windowHandle_, raw.data.mouse, window->eventQueue_);
                }
                else if (raw.header.dwType == RIM_TYPEKEYBOARD) {
                    translateKeyboardEvent(raw.data.keyboard, window->eventQueue_);
                }
            }
            break;
        }
        case WM_MOUSEMOVE: {
            if (window) {
                TRACKMOUSEEVENT tme = {};
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hWnd;
                ::TrackMouseEvent(&tme);

                window->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::MouseEnteredEvent,
                    .data = MousePositionEvent{},
                });
            }
            break;
        }
        case WM_MOUSELEAVE: {
            if (window) {
                window->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::MouseExitedEvent,
                    .data = MousePositionEvent{},
                });
            }
            break;
        }
        default:
            break;
        }

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
};

} // namespace

GameWindowWin32::GameWindowWin32() = default;

GameWindowWin32::~GameWindowWin32() = default;

std::tuple<std::shared_ptr<GameWindowWin32>, std::unique_ptr<Error>>
GameWindowWin32::create(
    HINSTANCE hInstance,
    int nCmdShow,
    HICON icon,
    HICON iconSmall,
    bool useOpenGL,
    const std::shared_ptr<SystemEventQueue>& eventQueue,
    const gpu::PresentationParameters& presentationParameters,
    const HighDPISettings& highDPI) noexcept
{
    auto window = std::make_shared<GameWindowWin32Impl>();
    if (auto err = window->initialize(
            hInstance,
            nCmdShow,
            icon,
            iconSmall,
            useOpenGL,
            eventQueue,
            presentationParameters,
            highDPI);
        err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(window), nullptr);
}

} // namespace pomdog::detail::win32
