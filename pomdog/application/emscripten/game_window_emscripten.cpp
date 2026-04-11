// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/emscripten/game_window_emscripten.h"
#include "pomdog/application/mouse_cursor.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace {

[[nodiscard]] std::string_view
toCSSCursorName(pomdog::MouseCursor cursor)
{
    using namespace std::string_view_literals;
    switch (cursor) {
    case pomdog::MouseCursor::Arrow:
        return "default"sv;
    case pomdog::MouseCursor::IBeam:
        return "text"sv;
    case pomdog::MouseCursor::PointingHand:
        return "pointer"sv;
    case pomdog::MouseCursor::ResizeVertical:
        return "ns-resize"sv;
    case pomdog::MouseCursor::ResizeHorizontal:
        return "ew-resize"sv;
    }
    return "default"sv;
}

void setCanvasCursor(const std::string& canvasSelector, std::string_view cursorName)
{
    auto script = pomdog::format(
        "var c = document.querySelector('{}'); if (c) {{ c.style.cursor = '{}'; }}",
        canvasSelector,
        cursorName);

    emscripten_run_script(script.c_str());
}

} // namespace

namespace pomdog::detail::emscripten {

GameWindowEmscripten::GameWindowEmscripten() noexcept = default;

GameWindowEmscripten::~GameWindowEmscripten() noexcept = default;

std::unique_ptr<Error>
GameWindowEmscripten::initialize(
    const std::string& targetCanvas,
    int width,
    int height) noexcept
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

bool GameWindowEmscripten::getAllowUserResizing() const
{
    return allowUserResizing_;
}

void GameWindowEmscripten::setAllowUserResizing(bool allowResizing)
{
    allowUserResizing_ = allowResizing;
}

std::string
GameWindowEmscripten::getTitle() const
{
    return title_;
}

void GameWindowEmscripten::setTitle(const std::string& titleIn)
{
    title_ = titleIn;
}

Rect2D
GameWindowEmscripten::getClientBounds() const
{
    return clientBounds_;
}

void GameWindowEmscripten::setClientBounds(const Rect2D& clientBoundsIn)
{
    clientBounds_ = clientBoundsIn;
    emscripten_set_canvas_element_size(
        targetCanvas_.c_str(),
        clientBounds_.width,
        clientBounds_.height);
    clientSizeChanged(clientBounds_.width, clientBounds_.height);
}

bool GameWindowEmscripten::isMouseCursorVisible() const
{
    return isMouseCursorVisible_;
}

void GameWindowEmscripten::setMouseCursorVisible(bool visible)
{
    isMouseCursorVisible_ = visible;

    if (isMouseCursorVisible_) {
        setCanvasCursor(targetCanvas_, toCSSCursorName(mouseCursor_));
    }
    else {
        setCanvasCursor(targetCanvas_, "none");
    }
}

void GameWindowEmscripten::setMouseCursor(MouseCursor cursor)
{
    mouseCursor_ = cursor;

    if (isMouseCursorVisible_) {
        setCanvasCursor(targetCanvas_, toCSSCursorName(mouseCursor_));
    }
}

const std::string&
GameWindowEmscripten::getTargetCanvas() const noexcept
{
    return targetCanvas_;
}

} // namespace pomdog::detail::emscripten
