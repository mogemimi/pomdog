// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem.GL4/OpenGLPrerequisites.hpp"
#include "Pomdog/Application/GameWindow.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <memory>
#include <string>

namespace Pomdog {
namespace Detail {
namespace X11 {

class X11Context;

class GameWindowX11 final : public GameWindow {
public:
    GameWindowX11(
        const std::shared_ptr<X11Context const>& x11Context,
        GLXFBConfig framebufferConfig,
        int width,
        int height);

    ~GameWindowX11();

    bool GetAllowUserResizing() const override;

    void SetAllowUserResizing(bool allowResizing) override;

    std::string GetTitle() const override;

    void SetTitle(const std::string& title) override;

    Rectangle GetClientBounds() const override;

    void SetClientBounds(const Rectangle& clientBounds) override;

    bool IsMouseCursorVisible() const override;

    void SetMouseCursorVisible(bool visible) override;

    void SetMouseCursor(MouseCursor cursor) override;

    ::Display* NativeDisplay() const;

    ::Window NativeWindow() const;

    GLXFBConfig GetFramebufferConfig() const;

    bool IsMinimized() const;

    void ProcessEvent(::XEvent & event);

private:
    std::shared_ptr<X11Context const> x11Context;
    int framebufferConfigID;
    ::Colormap colormap;
    ::Window window;
    std::string title;
    Rectangle clientBounds;
    MouseCursor mouseCursor;
    bool allowUserResizing;
    bool isMinimized;
    bool isMouseCursorVisible;
};

} // namespace X11
} // namespace Detail
} // namespace Pomdog
