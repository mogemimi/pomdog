// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_window.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::x11 {
class X11Context;
} // namespace pomdog::detail::x11

namespace pomdog::detail::x11 {

class GameWindowX11 : public GameWindow {
public:
    GameWindowX11();

    ~GameWindowX11() override;

    [[nodiscard]] virtual ::Display*
    getNativeDisplay() const = 0;

    [[nodiscard]] virtual ::Window
    getNativeWindow() const noexcept = 0;

    [[nodiscard]] virtual ::XIC
    getInputContext() const noexcept = 0;

    [[nodiscard]] virtual GLXFBConfig
    getFramebufferConfig() const = 0;

    [[nodiscard]] virtual bool
    isMinimized() const noexcept = 0;

    virtual void
    processEvent(::XEvent& event) = 0;

    /// Applies any pending window requests (mode, bounds, title, resizing).
    /// Called by `GameHostLinux` at the start of each frame, before `game.update()`.
    virtual void
    applyPendingWindowRequests() noexcept = 0;

    [[nodiscard]] static std::tuple<std::shared_ptr<GameWindowX11>, std::unique_ptr<Error>>
    create(
        const std::shared_ptr<X11Context const>& x11Context,
        GLXFBConfig framebufferConfig,
        int width,
        int height) noexcept;
};

} // namespace pomdog::detail::x11
