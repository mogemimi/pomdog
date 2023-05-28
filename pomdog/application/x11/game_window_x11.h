// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_window.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/utility/errors.h"
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <memory>
#include <string>

namespace pomdog::detail::x11 {

class X11Context;

class GameWindowX11 final : public GameWindow {
public:
    GameWindowX11();

    ~GameWindowX11() override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<X11Context const>& x11Context,
        GLXFBConfig framebufferConfig,
        int width,
        int height) noexcept;

    bool getAllowUserResizing() const override;

    void setAllowUserResizing(bool allowResizing) override;

    std::string getTitle() const override;

    void setTitle(const std::string& title) override;

    Rectangle getClientBounds() const override;

    void setClientBounds(const Rectangle& clientBounds) override;

    bool isMouseCursorVisible() const noexcept override;

    void setMouseCursorVisible(bool visible) override;

    void setMouseCursor(MouseCursor cursor) override;

    [[nodiscard]] ::Display*
    getNativeDisplay() const;

    [[nodiscard]] ::Window
    getNativeWindow() const noexcept;

    [[nodiscard]] ::XIC
    getInputContext() const noexcept;

    [[nodiscard]] GLXFBConfig
    getFramebufferConfig() const;

    [[nodiscard]] bool
    isMinimized() const noexcept;

    void processEvent(::XEvent& event);

private:
    std::shared_ptr<X11Context const> x11Context_;
    int framebufferConfigID_ = 0;
    ::Colormap colormap_ = 0;
    ::Window window_ = 0;
    ::XIM inputMethod_ = nullptr;
    ::XIC inputContext_ = nullptr;
    std::string title_;
    Rectangle clientBounds_;
    MouseCursor mouseCursor_;
    bool allowUserResizing_ = true;
    bool isMinimized_ = false;
    bool isMouseCursorVisible_ = true;
};

} // namespace pomdog::detail::x11
