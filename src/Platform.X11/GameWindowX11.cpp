// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GameWindowX11.hpp"
#include "X11Context.hpp"
#include "../Utility/ScopeGuard.hpp"
#include "Pomdog/Basic/Platform.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <X11/Xutil.h>

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
    , allowUserResizing(true)
    , isMinimized(false)
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
    ///@todo Not implemented
    //POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
    return true;
}

void GameWindowX11::SetMouseCursorVisible(bool /*visible*/)
{
    ///@todo Not implemented
    //POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GameWindowX11::SetMouseCursor(MouseCursor /*cursor*/)
{
    ///@todo Not implemented
    //POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
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
