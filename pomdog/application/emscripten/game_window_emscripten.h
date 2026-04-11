// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_window.h"
#include "pomdog/application/mouse_cursor.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/rect2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::emscripten {

class GameWindowEmscripten final : public GameWindow {
private:
    std::string targetCanvas_ = {};
    std::string title_ = {};
    Rect2D clientBounds_ = {};
    MouseCursor mouseCursor_ = MouseCursor::Arrow;
    bool allowUserResizing_ = false;
    bool isMouseCursorVisible_ = true;

public:
    GameWindowEmscripten() noexcept;

    ~GameWindowEmscripten() noexcept override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::string& targetCanvas, int width, int height) noexcept;

    [[nodiscard]] bool
    getAllowUserResizing() const override;

    void
    setAllowUserResizing(bool allowResizing) override;

    [[nodiscard]] std::string
    getTitle() const override;

    void
    setTitle(const std::string& title) override;

    [[nodiscard]] Rect2D
    getClientBounds() const override;

    void
    setClientBounds(const Rect2D& clientBounds) override;

    [[nodiscard]] bool
    isMouseCursorVisible() const override;

    void
    setMouseCursorVisible(bool visible) override;

    void
    setMouseCursor(MouseCursor cursor) override;

    [[nodiscard]] const std::string&
    getTargetCanvas() const noexcept;
};

} // namespace pomdog::detail::emscripten
