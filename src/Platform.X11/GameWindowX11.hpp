// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "../Graphics.GL4/OpenGLPrerequisites.hpp"
#include "Pomdog/Application/GameWindow.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <memory>
#include <string>

namespace Pomdog::Detail::X11 {

class X11Context;

class GameWindowX11 final : public GameWindow {
public:
    GameWindowX11();

    ~GameWindowX11() override;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        const std::shared_ptr<X11Context const>& x11Context,
        GLXFBConfig framebufferConfig,
        int width,
        int height) noexcept;

    bool GetAllowUserResizing() const override;

    void SetAllowUserResizing(bool allowResizing) override;

    std::string GetTitle() const override;

    void SetTitle(const std::string& title) override;

    Rectangle GetClientBounds() const override;

    void SetClientBounds(const Rectangle& clientBounds) override;

    bool IsMouseCursorVisible() const noexcept override;

    void SetMouseCursorVisible(bool visible) override;

    void SetMouseCursor(MouseCursor cursor) override;

    ::Display* GetNativeDisplay() const;

    ::Window GetNativeWindow() const noexcept;

    ::XIC GetInputContext() const noexcept;

    GLXFBConfig GetFramebufferConfig() const;

    bool IsMinimized() const noexcept;

    void ProcessEvent(::XEvent& event);

private:
    std::shared_ptr<X11Context const> x11Context;
    int framebufferConfigID = 0;
    ::Colormap colormap = 0;
    ::Window window = 0;
    ::XIM inputMethod = nullptr;
    ::XIC inputContext = nullptr;
    std::string title;
    Rectangle clientBounds;
    MouseCursor mouseCursor;
    bool allowUserResizing = true;
    bool isMinimized = false;
    bool isMouseCursorVisible = true;
};

} // namespace Pomdog::Detail::X11
