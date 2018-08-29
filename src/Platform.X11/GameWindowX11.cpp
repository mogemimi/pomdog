// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GameWindowX11.hpp"
#include "X11Context.hpp"
#include "../Utility/ScopeGuard.hpp"
#include "Pomdog/Application/MouseCursor.hpp"
#include "Pomdog/Basic/Platform.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <X11/cursorfont.h>
#include <X11/Xutil.h>
#include <optional>

namespace Pomdog {
namespace Detail {
namespace X11 {
namespace {

Rectangle GetWindowClientBounds(::Display* display, ::Window window)
{
    XWindowAttributes windowAttributes;
    XGetWindowAttributes(display, window, &windowAttributes);

    return Rectangle {
        windowAttributes.x, windowAttributes.y,
        windowAttributes.width, windowAttributes.height};
}

void UpdateNormalHints(::Display* display, ::Window window, int width, int height, bool allowResizing)
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

int ToMouseCursor(MouseCursor cursor) noexcept
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

void UpdateMouseCursor(::Display* display, ::Window window, std::optional<MouseCursor> mouseCursor)
{
    POMDOG_ASSERT(display != nullptr);
    POMDOG_ASSERT(window != None);

    if (!mouseCursor) {
        constexpr char data[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
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
        auto cursor = XCreateFontCursor(display, ToMouseCursor(*mouseCursor));
        XDefineCursor(display, window, cursor);
        XFreeCursor(display, cursor);
    }
}

} // unnamed namespace

GameWindowX11::GameWindowX11(
    const std::shared_ptr<X11Context const>& x11ContextIn,
    GLXFBConfig framebufferConfig,
    int width,
    int height)
    : x11Context(x11ContextIn)
    , framebufferConfigID(0)
    , colormap(0)
    , window(0)
    , mouseCursor(MouseCursor::Arrow)
    , allowUserResizing(true)
    , isMinimized(false)
    , isMouseCursorVisible(true)
{
    POMDOG_ASSERT(x11Context);

    auto display = x11Context->Display;

    glXGetFBConfigAttrib(display, framebufferConfig, GLX_FBCONFIG_ID, &framebufferConfigID);

    auto visualInfo = glXGetVisualFromFBConfig(display, framebufferConfig);

    if (visualInfo == nullptr) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Error: glXGetVisualFromFBConfig");
    }

    XLockDisplay(display);

    auto rootWindow = RootWindow(display, visualInfo->screen);

    colormap = XCreateColormap(display, rootWindow, visualInfo->visual, AllocNone);

    XSetWindowAttributes windowAttributes;
    windowAttributes.background_pixmap = None;
    windowAttributes.border_pixel = 0;
    windowAttributes.colormap = colormap;
    windowAttributes.event_mask = ExposureMask | FocusChangeMask | VisibilityChangeMask
        | StructureNotifyMask
        | EnterWindowMask | LeaveWindowMask
        | PropertyChangeMask
        | KeyPressMask | KeyReleaseMask
        | PointerMotionMask
        | ButtonPressMask | ButtonReleaseMask
        | ButtonMotionMask | Button1MotionMask;

    constexpr unsigned long windowAttributeMask = CWBorderPixel | CWColormap | CWEventMask;
    constexpr unsigned int borderWidth = 0;

    window = ::XCreateWindow(display, rootWindow,
        0, 0, width, height, borderWidth,
        visualInfo->depth, InputOutput, visualInfo->visual,
        windowAttributeMask, &windowAttributes);

    ::XFree(visualInfo);

    if (window == 0) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create window");
    }

    ::XMapWindow(display, window);
    //::XMapRaised(display, window);

    std::string windowName = "X11";
    ::XStoreName(display, window, windowName.c_str());

    const auto& atoms = x11Context->Atoms;
    auto wmDeleteWindow = atoms.WmDeleteWindow;
    XSetWMProtocols(display, window, &wmDeleteWindow, 1);

    XUnlockDisplay(x11Context->Display);

    clientBounds = GetWindowClientBounds(x11Context->Display, window);
}

GameWindowX11::~GameWindowX11()
{
    POMDOG_ASSERT(x11Context);
    auto display = x11Context->Display;

    POMDOG_ASSERT(display != nullptr);

    if (window != 0) {
        ::XUnmapWindow(display, window);
        ::XDestroyWindow(display, window);
        window = 0;
    }

    if (colormap != 0) {
        ::XFreeColormap(display, colormap);
        colormap = 0;
    }

    XFlush(display);
}

bool GameWindowX11::GetAllowUserResizing() const
{
    return this->allowUserResizing;
}

void GameWindowX11::SetAllowUserResizing(bool allowResizing)
{
    if (this->allowUserResizing == allowResizing) {
        return;
    }
    this->allowUserResizing = allowResizing;

    UpdateNormalHints(x11Context->Display, window,
        clientBounds.Width, clientBounds.Height, allowUserResizing);
}

std::string GameWindowX11::GetTitle() const
{
    return this->title;
}

void GameWindowX11::SetTitle(const std::string& titleIn)
{
    this->title = titleIn;

    POMDOG_ASSERT(x11Context);
    auto display = x11Context->Display;

#if defined(X_HAVE_UTF8_STRING)
    Xutf8SetWMProperties(display, window, title.c_str(), title.c_str(),
        nullptr, 0, nullptr, nullptr, nullptr);
#else
    XmbSetWMProperties(display, window, title.c_str(), title.c_str(),
        nullptr, 0, nullptr, nullptr, nullptr);
#endif

    const auto& atoms = x11Context->Atoms;

    if (atoms.NetWmName != None) {
        XChangeProperty(display, window, atoms.NetWmName,
            x11Context->Atoms.Utf8String, 8, PropModeReplace,
            reinterpret_cast<const unsigned char*>(title.c_str()),
            static_cast<int>(title.size()));
    }

    if (x11Context->Atoms.NetWmIconName != None) {
        XChangeProperty(display, window, atoms.NetWmIconName,
            atoms.Utf8String, 8, PropModeReplace,
            reinterpret_cast<const unsigned char*>(title.c_str()),
            static_cast<int>(title.size()));
    }

    XFlush(x11Context->Display);
}

Rectangle GameWindowX11::GetClientBounds() const
{
    return clientBounds;
}

void GameWindowX11::SetClientBounds(const Rectangle& clientBoundsIn)
{
    clientBounds = clientBoundsIn;

    XMoveWindow(x11Context->Display, window, clientBounds.X, clientBounds.Y);
    XResizeWindow(x11Context->Display, window, clientBounds.Width, clientBounds.Height);

    if (!allowUserResizing) {
        UpdateNormalHints(x11Context->Display, window,
            clientBounds.Width, clientBounds.Height, allowUserResizing);
    }

    XFlush(x11Context->Display);
}

bool GameWindowX11::IsMouseCursorVisible() const
{
    return isMouseCursorVisible;
}

void GameWindowX11::SetMouseCursorVisible(bool visible)
{
    if (isMouseCursorVisible == visible) {
        return;
    }
    isMouseCursorVisible = visible;

    if (isMouseCursorVisible) {
        UpdateMouseCursor(x11Context->Display, window, mouseCursor);
    }
    else {
        UpdateMouseCursor(x11Context->Display, window, std::nullopt);
    }
}

void GameWindowX11::SetMouseCursor(MouseCursor mouseCursorIn)
{
    if (mouseCursor == mouseCursorIn) {
        return;
    }
    mouseCursor = mouseCursorIn;

    if (isMouseCursorVisible) {
        UpdateMouseCursor(x11Context->Display, window, mouseCursor);
    }
}

::Display* GameWindowX11::NativeDisplay() const
{
    POMDOG_ASSERT(x11Context);
    auto display = x11Context->Display;
    return display;
}

::Window GameWindowX11::NativeWindow() const
{
    return window;
}

GLXFBConfig GameWindowX11::GetFramebufferConfig() const
{
    int framebufferConfigAttributes[] = {
        GLX_FBCONFIG_ID, framebufferConfigID,
        None
    };

    POMDOG_ASSERT(x11Context);
    auto display = x11Context->Display;

    int framebufferConfigCount = 0;
    auto framebufferConfigs = glXChooseFBConfig(display, DefaultScreen(display),
        framebufferConfigAttributes, &framebufferConfigCount);

    POMDOG_ASSERT(framebufferConfigs != nullptr);
    POMDOG_ASSERT(framebufferConfigCount == 1);

    GLXFBConfig framebufferConfig = framebufferConfigs[0];
    XFree(framebufferConfigs);

    return framebufferConfig;
}

bool GameWindowX11::IsMinimized() const
{
    return isMinimized;
}

void GameWindowX11::ProcessEvent(::XEvent & event)
{
    bool clientSizeChanged = false;

    switch (event.type) {
    case ConfigureNotify: {
        clientBounds.X = event.xconfigure.x;
        clientBounds.Y = event.xconfigure.y;

        if (clientBounds.Width != event.xconfigure.width
            || clientBounds.Height != event.xconfigure.height) {
            clientBounds.Width = event.xconfigure.width;
            clientBounds.Height = event.xconfigure.height;
            clientSizeChanged = true;
        }
        break;
    }
    case MapNotify: {
        this->isMinimized = false;
        Log::Internal("GameWindow.IsMinimized = false");
        break;
    }
    case UnmapNotify: {
        this->isMinimized = true;
        Log::Internal("GameWindow.IsMinimized = true");
        break;
    }
//    case Expose: {
//        break;
//    }
//    case FocusIn: {
//        if (event.xfocus.mode == NotifyNormal) {
//            ///@todo
//        }
//        break;
//    }
//    case FocusOut: {
//        if (event.xfocus.mode == NotifyNormal) {
//            ///@todo
//        }
//        break;
//    }
    default:
        break;
    }

    if (clientSizeChanged) {
        this->ClientSizeChanged(clientBounds.Width, clientBounds.Height);
    }
}

} // namespace X11
} // namespace Detail
} // namespace Pomdog
