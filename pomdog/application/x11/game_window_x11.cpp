// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/x11/game_window_x11.h"
#include "pomdog/application/mouse_cursor.h"
#include "pomdog/application/x11/x11_context.h"
#include "pomdog/basic/platform.h"
#include "pomdog/logging/log.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/scope_guard.h"
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <optional>
#include <utility>

namespace pomdog::detail::x11 {
namespace {

[[nodiscard]] Rectangle getWindowClientBounds(::Display* display, ::Window window)
{
    XWindowAttributes windowAttributes;
    XGetWindowAttributes(display, window, &windowAttributes);

    return Rectangle{
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

[[nodiscard]] int toMouseCursor(MouseCursor cursor) noexcept
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

} // namespace

GameWindowX11::GameWindowX11() = default;

std::unique_ptr<Error>
GameWindowX11::initialize(
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
    //::XMapRaised(display, window_);

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
            // NOTE: XIM can't get styles.
            return false;
        }
        detail::ScopeGuard defer([&] {
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

GameWindowX11::~GameWindowX11()
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

bool GameWindowX11::getAllowUserResizing() const
{
    return allowUserResizing_;
}

void GameWindowX11::setAllowUserResizing(bool allowResizing)
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

std::string GameWindowX11::getTitle() const
{
    return title_;
}

void GameWindowX11::setTitle(const std::string& titleIn)
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

Rectangle GameWindowX11::getClientBounds() const
{
    return clientBounds_;
}

void GameWindowX11::setClientBounds(const Rectangle& clientBoundsIn)
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

bool GameWindowX11::isMouseCursorVisible() const noexcept
{
    return isMouseCursorVisible_;
}

void GameWindowX11::setMouseCursorVisible(bool visible)
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

void GameWindowX11::setMouseCursor(MouseCursor mouseCursorIn)
{
    if (mouseCursor_ == mouseCursorIn) {
        return;
    }
    mouseCursor_ = mouseCursorIn;

    if (isMouseCursorVisible_) {
        updateMouseCursor(x11Context_->Display, window_, mouseCursor_);
    }
}

::Display* GameWindowX11::getNativeDisplay() const
{
    POMDOG_ASSERT(x11Context_);
    const auto display = x11Context_->Display;
    return display;
}

::Window GameWindowX11::getNativeWindow() const noexcept
{
    return window_;
}

::XIC GameWindowX11::getInputContext() const noexcept
{
    return inputContext_;
}

GLXFBConfig GameWindowX11::getFramebufferConfig() const
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

bool GameWindowX11::isMinimized() const noexcept
{
    return isMinimized_;
}

void GameWindowX11::processEvent(::XEvent& event)
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
    // case Expose: {
    //     break;
    // }
    // case FocusIn: {
    //     if (event.xfocus.mode == NotifyNormal) {
    //         ///@todo
    //     }
    //     break;
    // }
    // case FocusOut: {
    //     if (event.xfocus.mode == NotifyNormal) {
    //         ///@todo
    //     }
    //     break;
    // }
    default:
        break;
    }

    if (requestClientSizeChangedEvent) {
        clientSizeChanged(clientBounds_.width, clientBounds_.height);
    }
}

} // namespace pomdog::detail::x11
