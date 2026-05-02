// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/x11/game_window_x11.h"
#include "pomdog/application/mouse_cursor.h"
#include "pomdog/application/window_mode.h"
#include "pomdog/application/x11/x11_context.h"
#include "pomdog/basic/platform.h"
#include "pomdog/logging/log.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/scope_guard.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <optional>
#include <utility>
#include <vector>

namespace pomdog::detail::x11 {
namespace {

[[nodiscard]] Rect2D
getWindowClientBounds(::Display* display, ::Window window)
{
    XWindowAttributes windowAttributes;
    XGetWindowAttributes(display, window, &windowAttributes);

    return Rect2D{
        windowAttributes.x,
        windowAttributes.y,
        windowAttributes.width,
        windowAttributes.height,
    };
}

void updateNormalHints(::Display* display, ::Window window, int width, int height, bool allowResizing)
{
    auto hints = XAllocSizeHints();
    POMDOG_ASSERT(hints != nullptr);

    hints->flags |= (PMinSize | PMaxSize);
    if (allowResizing) {
        hints->min_width = 1;
        hints->max_width = 16384;
        hints->min_height = 1;
        hints->max_height = 16384;
    }
    else {
        hints->min_width = width;
        hints->max_width = width;
        hints->min_height = height;
        hints->max_height = height;
    }

    hints->flags |= PWinGravity;
    hints->win_gravity = StaticGravity;

    XSetWMNormalHints(display, window, hints);
    XFree(hints);
}

[[nodiscard]] int
toMouseCursor(MouseCursor cursor) noexcept
{
    switch (cursor) {
    case MouseCursor::Arrow:
        return XC_left_ptr;
    case MouseCursor::IBeam:
        return XC_xterm;
    case MouseCursor::PointingHand:
        return XC_hand1;
    case MouseCursor::ResizeHorizontal:
        return XC_sb_h_double_arrow;
    case MouseCursor::ResizeVertical:
        return XC_sb_v_double_arrow;
    }
    return XC_arrow;
}

void updateMouseCursor(::Display* display, ::Window window, std::optional<MouseCursor> mouseCursor)
{
    POMDOG_ASSERT(display != nullptr);
    POMDOG_ASSERT(window != None);

    if (!mouseCursor) {
        constexpr char data[] = {0, 0, 0, 0, 0, 0, 0, 0};
        XColor black;
        black.red = 0;
        black.green = 0;
        black.blue = 0;
        auto blankBitmap = XCreateBitmapFromData(display, window, data, 8, 8);
        POMDOG_ASSERT(blankBitmap != None);

        auto cursor = XCreatePixmapCursor(display, blankBitmap, blankBitmap, &black, &black, 0, 0);
        XDefineCursor(display, window, cursor);
        XFreeCursor(display, cursor);

        XFreePixmap(display, blankBitmap);
        return;
    }

    if (*mouseCursor == MouseCursor::Arrow) {
        XUndefineCursor(display, window);
    }
    else {
        auto cursor = XCreateFontCursor(display, toMouseCursor(*mouseCursor));
        XDefineCursor(display, window, cursor);
        XFreeCursor(display, cursor);
    }
}

void setMotifWmDecorations(::Display* display, ::Window window, Atom motifWmHints, bool decorated) noexcept
{
    if (motifWmHints == None) {
        return;
    }

    struct MotifHints final {
        unsigned long flags = 0;
        unsigned long functions = 0;
        unsigned long decorations = 0;
        long inputMode = 0;
        unsigned long status = 0;
    };

    MotifHints hints = {};
    hints.flags = 2; // MWM_HINTS_DECORATIONS
    hints.decorations = decorated ? 1 : 0;

    XChangeProperty(
        display,
        window,
        motifWmHints,
        motifWmHints,
        32,
        PropModeReplace,
        reinterpret_cast<unsigned char*>(&hints),
        5);

    XFlush(display);
}

void sendNetWmStateMessage(
    ::Display* display,
    ::Window window,
    bool add,
    Atom atom1,
    Atom atom2 = None) noexcept
{
    auto rootWindow = DefaultRootWindow(display);

    ::XClientMessageEvent msg = {};
    msg.type = ClientMessage;
    msg.window = window;
    msg.message_type = XInternAtom(display, "_NET_WM_STATE", False);
    msg.format = 32;
    msg.data.l[0] = add ? 1 : 0; // 1 = _NET_WM_STATE_ADD, 0 = _NET_WM_STATE_REMOVE
    msg.data.l[1] = static_cast<long>(atom1);
    msg.data.l[2] = static_cast<long>(atom2);
    msg.data.l[3] = 1; // source indication: application

    ::XSendEvent(
        display,
        rootWindow,
        False,
        SubstructureNotifyMask | SubstructureRedirectMask,
        reinterpret_cast<::XEvent*>(&msg));

    ::XFlush(display);
}

[[nodiscard]] std::vector<Atom>
getNetWmStateAtoms(::Display* display, ::Window window) noexcept
{
    std::vector<Atom> result;

    Atom actualType = None;
    int actualFormat = 0;
    unsigned long itemCount = 0;
    unsigned long bytesRemaining = 0;
    unsigned char* propData = nullptr;

    auto netWmState = XInternAtom(display, "_NET_WM_STATE", True);
    if (netWmState == None) {
        return result;
    }

    const int status = ::XGetWindowProperty(
        display,
        window,
        netWmState,
        0,
        1024,
        False,
        XA_ATOM,
        &actualType,
        &actualFormat,
        &itemCount,
        &bytesRemaining,
        &propData);

    if (status == Success && propData != nullptr) {
        auto* atoms = reinterpret_cast<Atom*>(propData);
        result.assign(atoms, atoms + itemCount);
        ::XFree(propData);
    }

    return result;
}

class GameWindowX11Impl final : public GameWindowX11 {
private:
    std::shared_ptr<X11Context const> x11Context_;
    int framebufferConfigID_ = 0;
    ::Colormap colormap_ = 0;
    ::Window window_ = 0;
    ::XIM inputMethod_ = nullptr;
    ::XIC inputContext_ = nullptr;
    std::string title_;
    Rect2D clientBounds_;
    MouseCursor mouseCursor_;
    WindowMode windowMode_ = WindowMode::Windowed;
    bool allowUserResizing_ = true;
    bool isMinimized_ = false;
    bool isMouseCursorVisible_ = true;

    // NOTE: Pending requests - set by `request...()` and applied at the next
    //       `applyPendingWindowRequests()` call at the frame boundary.
    std::optional<WindowMode> pendingWindowMode_;
    std::optional<Rect2D> pendingClientBounds_;
    std::optional<std::string> pendingTitle_;
    std::optional<bool> pendingAllowUserResizing_;

public:
    ~GameWindowX11Impl() override
    {
        POMDOG_ASSERT(x11Context_);
        const auto display = x11Context_->Display;

        POMDOG_ASSERT(display != nullptr);

        if (inputContext_ != nullptr) {
            ::XDestroyIC(inputContext_);
            inputContext_ = nullptr;
        }

        if (inputMethod_ != nullptr) {
            ::XCloseIM(inputMethod_);
            inputMethod_ = nullptr;
        }

        if (window_ != 0) {
            ::XUnmapWindow(display, window_);
            ::XDestroyWindow(display, window_);
            window_ = 0;
        }

        if (colormap_ != 0) {
            ::XFreeColormap(display, colormap_);
            colormap_ = 0;
        }

        XFlush(display);
    }

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<X11Context const>& x11ContextIn,
        GLXFBConfig framebufferConfig,
        int width,
        int height) noexcept
    {
        x11Context_ = x11ContextIn;
        framebufferConfigID_ = 0;
        colormap_ = 0;
        window_ = 0;
        inputMethod_ = nullptr;
        inputContext_ = nullptr;
        mouseCursor_ = MouseCursor::Arrow;
        allowUserResizing_ = true;
        isMinimized_ = false;
        isMouseCursorVisible_ = true;

        POMDOG_ASSERT(x11Context_);

        const auto display = x11Context_->Display;

        glXGetFBConfigAttrib(display, framebufferConfig, GLX_FBCONFIG_ID, &framebufferConfigID_);

        auto visualInfo = glXGetVisualFromFBConfig(display, framebufferConfig);
        if (visualInfo == nullptr) {
            return errors::make("glXGetVisualFromFBConfig() failed");
        }

        XLockDisplay(display);

        auto rootWindow = RootWindow(display, visualInfo->screen);

        colormap_ = XCreateColormap(display, rootWindow, visualInfo->visual, AllocNone);

        XSetWindowAttributes windowAttributes;
        windowAttributes.background_pixmap = None;
        windowAttributes.border_pixel = 0;
        windowAttributes.colormap = colormap_;
        windowAttributes.event_mask =
            ExposureMask | FocusChangeMask | VisibilityChangeMask |
            StructureNotifyMask |
            EnterWindowMask | LeaveWindowMask |
            PropertyChangeMask |
            KeyPressMask | KeyReleaseMask |
            PointerMotionMask |
            ButtonPressMask | ButtonReleaseMask |
            ButtonMotionMask | Button1MotionMask;

        constexpr unsigned long windowAttributeMask = CWBorderPixel | CWColormap | CWEventMask;
        constexpr unsigned int borderWidth = 0;

        window_ = ::XCreateWindow(
            display,
            rootWindow,
            0,
            0,
            width,
            height,
            borderWidth,
            visualInfo->depth,
            InputOutput,
            visualInfo->visual,
            windowAttributeMask,
            &windowAttributes);

        ::XFree(visualInfo);

        if (window_ == 0) {
            return errors::make("XCreateWindow() failed");
        }

        // NOTE: Put the window on screen
        ::XMapWindow(display, window_);

        std::string windowName = "X11";
        ::XStoreName(display, window_, windowName.c_str());

        const auto& atoms = x11Context_->Atoms;
        auto wmDeleteWindow = atoms.WmDeleteWindow;
        XSetWMProtocols(display, window_, &wmDeleteWindow, 1);

        XUnlockDisplay(x11Context_->Display);

        clientBounds_ = getWindowClientBounds(x11Context_->Display, window_);

        inputMethod_ = ::XOpenIM(display, nullptr, nullptr, nullptr);
        if (inputMethod_ == nullptr) {
            return errors::make("could not open input method");
        }

        const auto hasInputMethodStyle = [&]() -> bool {
            ::XIMStyles* styles = nullptr;
            if (::XGetIMValues(inputMethod_, XNQueryInputStyle, &styles, nullptr) != nullptr) {
                return false;
            }
            ScopeGuard defer([&] {
                ::XFree(styles);
            });

            if (styles->supported_styles == nullptr) {
                return false;
            }
            for (int i = 0; i < styles->count_styles; i++) {
                if (styles->supported_styles[i] == (XIMPreeditNothing | XIMStatusNothing)) {
                    return true;
                }
            }
            return false;
        }();

        if (!hasInputMethodStyle) {
            return errors::make("XIM can't get styles");
        }

        inputContext_ = ::XCreateIC(
            inputMethod_,
            XNInputStyle,
            XIMPreeditNothing | XIMStatusNothing,
            XNClientWindow,
            window_,
            nullptr);

        if (inputContext_ == nullptr) {
            return errors::make("could not open input context");
        }

        ::XSetICFocus(inputContext_);

        return nullptr;
    }

    bool
    getAllowUserResizing() const override
    {
        return allowUserResizing_;
    }

    void
    requestAllowUserResizing(bool allowResizing) override
    {
        pendingAllowUserResizing_ = allowResizing;
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
        return clientBounds_;
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

    bool
    isMouseCursorVisible() const noexcept override
    {
        return isMouseCursorVisible_;
    }

    void
    setMouseCursorVisible(bool visible) override
    {
        if (isMouseCursorVisible_ == visible) {
            return;
        }
        isMouseCursorVisible_ = visible;

        if (isMouseCursorVisible_) {
            updateMouseCursor(x11Context_->Display, window_, mouseCursor_);
        }
        else {
            updateMouseCursor(x11Context_->Display, window_, std::nullopt);
        }
    }

    void
    setMouseCursor(MouseCursor mouseCursorIn) override
    {
        if (mouseCursor_ == mouseCursorIn) {
            return;
        }
        mouseCursor_ = mouseCursorIn;

        if (isMouseCursorVisible_) {
            updateMouseCursor(x11Context_->Display, window_, mouseCursor_);
        }
    }

    ::Display*
    getNativeDisplay() const override
    {
        POMDOG_ASSERT(x11Context_);
        return x11Context_->Display;
    }

    ::Window
    getNativeWindow() const noexcept override
    {
        return window_;
    }

    ::XIC
    getInputContext() const noexcept override
    {
        return inputContext_;
    }

    GLXFBConfig
    getFramebufferConfig() const override
    {
        const int framebufferConfigAttributes[] = {
            GLX_FBCONFIG_ID,
            framebufferConfigID_,
            None,
        };

        POMDOG_ASSERT(x11Context_);
        const auto display = x11Context_->Display;

        int framebufferConfigCount = 0;
        auto framebufferConfigs = glXChooseFBConfig(
            display,
            DefaultScreen(display),
            framebufferConfigAttributes,
            &framebufferConfigCount);

        POMDOG_ASSERT(framebufferConfigs != nullptr);
        POMDOG_ASSERT(framebufferConfigCount == 1);

        GLXFBConfig framebufferConfig = framebufferConfigs[0];
        XFree(framebufferConfigs);

        return framebufferConfig;
    }

    bool
    isMinimized() const noexcept override
    {
        return isMinimized_;
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
            return errors::make("BrowserSoftFullscreen mode is not supported on Linux.");
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

private:
    [[nodiscard]] std::unique_ptr<Error>
    applyWindowMode(WindowMode windowMode) noexcept
    {
        POMDOG_ASSERT(x11Context_);
        const auto display = x11Context_->Display;
        const auto& atoms = x11Context_->Atoms;

        if (windowMode == windowMode_) {
            return nullptr;
        }

        switch (windowMode) {
        case WindowMode::Fullscreen: {
            if (atoms.NetWmStateFullscreen == None) {
                return errors::make("_NET_WM_STATE_FULLSCREEN is not supported by this window manager.");
            }
            // NOTE: If currently borderless, restore decorations first.
            if (windowMode_ == WindowMode::BorderlessWindowed) {
                setMotifWmDecorations(display, window_, atoms.MotifWmHints, true);
            }
            sendNetWmStateMessage(display, window_, true, atoms.NetWmStateFullscreen);
            break;
        }
        case WindowMode::Maximized: {
            if (atoms.NetWmStateMaximizedVert == None || atoms.NetWmStateMaximizedHorz == None) {
                return errors::make("_NET_WM_STATE_MAXIMIZED is not supported by this window manager.");
            }
            // NOTE: First exit fullscreen if currently in fullscreen.
            if (windowMode_ == WindowMode::Fullscreen && atoms.NetWmStateFullscreen != None) {
                sendNetWmStateMessage(display, window_, false, atoms.NetWmStateFullscreen);
            }
            // NOTE: If currently borderless, restore decorations first.
            if (windowMode_ == WindowMode::BorderlessWindowed) {
                setMotifWmDecorations(display, window_, atoms.MotifWmHints, true);
            }
            sendNetWmStateMessage(
                display, window_, true,
                atoms.NetWmStateMaximizedVert,
                atoms.NetWmStateMaximizedHorz);
            break;
        }
        case WindowMode::Windowed: {
            // NOTE: Remove fullscreen and maximize states.
            if (windowMode_ == WindowMode::Fullscreen && atoms.NetWmStateFullscreen != None) {
                sendNetWmStateMessage(display, window_, false, atoms.NetWmStateFullscreen);
            }
            else if (windowMode_ == WindowMode::Maximized &&
                     atoms.NetWmStateMaximizedVert != None &&
                     atoms.NetWmStateMaximizedHorz != None) {
                sendNetWmStateMessage(
                    display, window_, false,
                    atoms.NetWmStateMaximizedVert,
                    atoms.NetWmStateMaximizedHorz);
            }
            // NOTE: If currently borderless, restore decorations.
            if (windowMode_ == WindowMode::BorderlessWindowed) {
                setMotifWmDecorations(display, window_, atoms.MotifWmHints, true);
            }
            break;
        }
        case WindowMode::BorderlessWindowed: {
            // NOTE: Remove fullscreen and maximize states first.
            if (windowMode_ == WindowMode::Fullscreen && atoms.NetWmStateFullscreen != None) {
                sendNetWmStateMessage(display, window_, false, atoms.NetWmStateFullscreen);
            }
            else if (windowMode_ == WindowMode::Maximized &&
                     atoms.NetWmStateMaximizedVert != None &&
                     atoms.NetWmStateMaximizedHorz != None) {
                sendNetWmStateMessage(
                    display, window_, false,
                    atoms.NetWmStateMaximizedVert,
                    atoms.NetWmStateMaximizedHorz);
            }
            // NOTE: Remove window decorations via Motif hints (best effort).
            setMotifWmDecorations(display, window_, atoms.MotifWmHints, false);
            break;
        }
        case WindowMode::BrowserSoftFullscreen:
            // NOTE: Validated and rejected in requestWindowMode(); should not reach here.
            break;
        }

        // NOTE: Optimistically update the mode; PropertyNotify will confirm.
        windowMode_ = windowMode;
        windowModeChanged(windowMode_);
        return nullptr;
    }

    void
    applyClientBounds(const Rect2D& clientBoundsIn) noexcept
    {
        clientBounds_ = clientBoundsIn;

        XMoveWindow(x11Context_->Display, window_, clientBounds_.x, clientBounds_.y);
        XResizeWindow(x11Context_->Display, window_, clientBounds_.width, clientBounds_.height);

        if (!allowUserResizing_) {
            updateNormalHints(
                x11Context_->Display,
                window_,
                clientBounds_.width,
                clientBounds_.height,
                allowUserResizing_);
        }

        XFlush(x11Context_->Display);
    }

    void
    applyTitle(const std::string& titleIn) noexcept
    {
        title_ = titleIn;

        POMDOG_ASSERT(x11Context_);
        const auto display = x11Context_->Display;

#if defined(X_HAVE_UTF8_STRING)
        Xutf8SetWMProperties(display, window_, title_.c_str(), title_.c_str(),
            nullptr, 0, nullptr, nullptr, nullptr);
#else
        XmbSetWMProperties(display, window_, title_.c_str(), title_.c_str(),
            nullptr, 0, nullptr, nullptr, nullptr);
#endif

        const auto& atoms = x11Context_->Atoms;

        if (atoms.NetWmName != None) {
            XChangeProperty(
                display,
                window_,
                atoms.NetWmName,
                x11Context_->Atoms.Utf8String, 8, PropModeReplace,
                reinterpret_cast<const unsigned char*>(title_.c_str()),
                static_cast<int>(title_.size()));
        }

        if (x11Context_->Atoms.NetWmIconName != None) {
            XChangeProperty(
                display,
                window_,
                atoms.NetWmIconName,
                atoms.Utf8String, 8, PropModeReplace,
                reinterpret_cast<const unsigned char*>(title_.c_str()),
                static_cast<int>(title_.size()));
        }

        XFlush(x11Context_->Display);
    }

    void
    applyAllowUserResizing(bool allowResizing) noexcept
    {
        if (allowUserResizing_ == allowResizing) {
            return;
        }
        allowUserResizing_ = allowResizing;

        updateNormalHints(
            x11Context_->Display,
            window_,
            clientBounds_.width,
            clientBounds_.height,
            allowUserResizing_);
    }

public:
    void
    processEvent(::XEvent& event) override
    {
        bool requestClientSizeChangedEvent = false;

        switch (event.type) {
        case ConfigureNotify: {
            clientBounds_.x = event.xconfigure.x;
            clientBounds_.y = event.xconfigure.y;

            if (clientBounds_.width != event.xconfigure.width ||
                clientBounds_.height != event.xconfigure.height) {
                clientBounds_.width = event.xconfigure.width;
                clientBounds_.height = event.xconfigure.height;
                requestClientSizeChangedEvent = true;
            }
            break;
        }
        case MapNotify: {
            isMinimized_ = false;
            break;
        }
        case UnmapNotify: {
            isMinimized_ = true;
            break;
        }
        case PropertyNotify: {
            // NOTE: Detect window manager-driven mode changes (e.g., user maximizes via taskbar).
            const auto& atoms = x11Context_->Atoms;
            if (event.xproperty.atom == atoms.NetWmState) {
                const auto stateAtoms = getNetWmStateAtoms(x11Context_->Display, window_);

                const bool isFullscreen =
                    atoms.NetWmStateFullscreen != None &&
                    std::find(stateAtoms.begin(), stateAtoms.end(), atoms.NetWmStateFullscreen) != stateAtoms.end();
                const bool isMaximized =
                    atoms.NetWmStateMaximizedVert != None &&
                    atoms.NetWmStateMaximizedHorz != None &&
                    std::find(stateAtoms.begin(), stateAtoms.end(), atoms.NetWmStateMaximizedVert) != stateAtoms.end() &&
                    std::find(stateAtoms.begin(), stateAtoms.end(), atoms.NetWmStateMaximizedHorz) != stateAtoms.end();

                WindowMode newMode = WindowMode::Windowed;
                if (isFullscreen) {
                    newMode = WindowMode::Fullscreen;
                }
                else if (isMaximized) {
                    newMode = WindowMode::Maximized;
                }

                if (newMode != windowMode_) {
                    windowMode_ = newMode;
                    windowModeChanged(windowMode_);
                    requestClientSizeChangedEvent = true;
                }
            }
            break;
        }
        default:
            break;
        }

        if (requestClientSizeChangedEvent) {
            clientSizeChanged(clientBounds_.width, clientBounds_.height);
        }
    }
};

} // namespace

GameWindowX11::GameWindowX11() = default;

GameWindowX11::~GameWindowX11() = default;

std::tuple<std::shared_ptr<GameWindowX11>, std::unique_ptr<Error>>
GameWindowX11::create(
    const std::shared_ptr<X11Context const>& x11Context,
    GLXFBConfig framebufferConfig,
    int width,
    int height) noexcept
{
    auto window = std::make_shared<GameWindowX11Impl>();
    if (auto err = window->initialize(x11Context, framebufferConfig, width, height); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(window), nullptr);
}

} // namespace pomdog::detail::x11
