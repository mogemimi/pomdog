// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/color.h"
#include "pomdog/math/f32_rect2d.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
struct Response;
struct Style;
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui::detail {

/// Holds a number formatted for display without allocating.
using NumberText = std::array<char, 32>;

/// The distance the pointer moves a held tab or row before it is dragged, so
/// that a click never reorders anything.
inline constexpr f32 reorderDragThreshold = 4.0f;

/// Describes a row split into a label column and a control area.
struct PropertyRow final {
    F32Rect2D row = {0.0f, 0.0f, 0.0f, 0.0f};
    F32Rect2D control = {0.0f, 0.0f, 0.0f, 0.0f};
};

/// Selects the direction a chevron points.
enum class ChevronDirection : u8 {
    Right,
    Down,
};

/// Allocates a row of the given height, draws label in its label column, and
/// returns the area left for the control. An empty label gives the control the
/// whole row. A label too long for its column ends with an ellipsis.
[[nodiscard]] PropertyRow
layoutPropertyRow(UI& ui, std::string_view label, f32 height);

/// Allocates one framed row like the overload above.
[[nodiscard]] PropertyRow
layoutPropertyRow(UI& ui, std::string_view label);

/// Formats value with the given number of decimal places into buffer.
[[nodiscard]] std::string_view
formatNumber(NumberText& buffer, f32 value, i32 decimals) noexcept;

/// Formats value into buffer.
[[nodiscard]] std::string_view
formatNumber(NumberText& buffer, i32 value) noexcept;

/// Returns the y position of single-line text centered vertically in rect.
[[nodiscard]] f32
getTextTop(const Style& style, const F32Rect2D& rect) noexcept;

/// Draws text centered horizontally and vertically in rect.
void drawCenteredText(UI& ui, const F32Rect2D& rect, std::string_view text, const Color& color);

/// Fills rect with fill inside a one-pixel border of border color.
void drawFrame(UI& ui, const F32Rect2D& rect, const Color& fill, const Color& border, f32 radius);

/// Returns the border color of an input-like widget in the state response
/// reports. A focused widget gets the accent color.
[[nodiscard]] Color
getInputBorderColor(const Style& style, const Response& response, bool focused) noexcept;

/// Draws a chevron of the given half size centered at center.
void drawChevron(UI& ui, const Vector2& center, f32 size, ChevronDirection direction, const Color& color);

/// Draws a check mark that fits in box.
void drawCheckMark(UI& ui, const F32Rect2D& box, const Color& color);

/// Draws a close button in rect in the state response reports: a cross,
/// shown when visible is true or the button is hovered, over a highlight
/// while the button is hovered.
void drawCloseButton(UI& ui, const F32Rect2D& rect, const Response& response, bool visible);

/// Draws a translucent copy of a dragged tab or row above everything, in
/// rect, with label textPadding pixels from its left edge.
void drawDragGhost(UI& ui, const F32Rect2D& rect, std::string_view label, f32 textPadding);

/// Draws the line that shows where a dragged item will go, centered on the
/// edge between two items: a vertical line at x when vertical is true, and a
/// horizontal line at y otherwise. extent is the rectangle the line spans.
void drawDropIndicator(UI& ui, const F32Rect2D& extent, f32 position, bool vertical);

} // namespace pomdog::devgui::detail
