// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/emscripten/game_window_emscripten.h"
#include "pomdog/application/mouse_cursor.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::emscripten {
namespace {

[[nodiscard]] std::string_view
toCSSCursorName(MouseCursor cursor)
{
    using namespace std::string_view_literals;
    switch (cursor) {
    case MouseCursor::Arrow:
        return "default"sv;
    case MouseCursor::IBeam:
        return "text"sv;
    case MouseCursor::PointingHand:
        return "pointer"sv;
    case MouseCursor::ResizeVertical:
        return "ns-resize"sv;
    case MouseCursor::ResizeHorizontal:
        return "ew-resize"sv;
    }
    return "default"sv;
}

void setCanvasCursor(const std::string& canvasSelector, std::string_view cursorName)
{
    auto script = format(
        "var c = document.querySelector('{}'); if (c) {{ c.style.cursor = '{}'; }}",
        canvasSelector,
        cursorName);

    emscripten_run_script(script.c_str());
}

class GameWindowEmscriptenImpl final : public GameWindowEmscripten {
private:
    std::string targetCanvas_ = {};
    std::string title_ = {};
    Rect2D clientBounds_ = {};
    MouseCursor mouseCursor_ = MouseCursor::Arrow;
    bool allowUserResizing_ = false;
    bool isMouseCursorVisible_ = true;

public:
    ~GameWindowEmscriptenImpl() override = default;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::string& targetCanvas, int width, int height) noexcept
    {
        targetCanvas_ = targetCanvas;

        if (width > 0 && height > 0) {
            emscripten_set_canvas_element_size(targetCanvas_.c_str(), width, height);
        }

        f64 cssWidth = 0.0;
        f64 cssHeight = 0.0;
        emscripten_get_element_css_size(targetCanvas_.c_str(), &cssWidth, &cssHeight);

        clientBounds_.x = 0;
        clientBounds_.y = 0;
        clientBounds_.width = static_cast<i32>(cssWidth);
        clientBounds_.height = static_cast<i32>(cssHeight);

        return nullptr;
    }

    bool
    getAllowUserResizing() const override
    {
        return allowUserResizing_;
    }

    void setAllowUserResizing(bool allowResizing) override
    {
        allowUserResizing_ = allowResizing;
    }

    std::string
    getTitle() const override
    {
        return title_;
    }

    void setTitle(const std::string& titleIn) override
    {
        title_ = titleIn;
    }

    Rect2D
    getClientBounds() const override
    {
        return clientBounds_;
    }

    void setClientBounds(const Rect2D& clientBoundsIn) override
    {
        clientBounds_ = clientBoundsIn;
        emscripten_set_canvas_element_size(
            targetCanvas_.c_str(),
            clientBounds_.width,
            clientBounds_.height);
        clientSizeChanged(clientBounds_.width, clientBounds_.height);
    }

    bool
    isMouseCursorVisible() const override
    {
        return isMouseCursorVisible_;
    }

    void setMouseCursorVisible(bool visible) override
    {
        isMouseCursorVisible_ = visible;

        if (isMouseCursorVisible_) {
            setCanvasCursor(targetCanvas_, toCSSCursorName(mouseCursor_));
        }
        else {
            setCanvasCursor(targetCanvas_, "none");
        }
    }

    void setMouseCursor(MouseCursor cursor) override
    {
        mouseCursor_ = cursor;

        if (isMouseCursorVisible_) {
            setCanvasCursor(targetCanvas_, toCSSCursorName(mouseCursor_));
        }
    }

    const std::string&
    getTargetCanvas() const noexcept override
    {
        return targetCanvas_;
    }
};

} // namespace

GameWindowEmscripten::GameWindowEmscripten() noexcept = default;

GameWindowEmscripten::~GameWindowEmscripten() noexcept = default;

std::tuple<std::shared_ptr<GameWindowEmscripten>, std::unique_ptr<Error>>
GameWindowEmscripten::create(const std::string& targetCanvas, int width, int height) noexcept
{
    auto window = std::make_shared<GameWindowEmscriptenImpl>();
    if (auto err = window->initialize(targetCanvas, width, height); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(window), nullptr);
}

} // namespace pomdog::detail::emscripten
