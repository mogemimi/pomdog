// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/color.h"
#include "pomdog/math/f32_rect2d.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {

enum class DrawCommandKind : u8 {
    /// Fills the rectangle at p0 with size p1.
    FillRect,

    /// Outlines the rectangle at p0 with size p1, thickness pixels wide inside it.
    StrokeRect,

    /// Fills the triangle p0, p1, p2.
    FillTriangle,

    /// Draws a line from p0 to p1, thickness pixels wide.
    Line,

    /// Draws a single line of text whose top-left corner is at p0.
    Text,

    /// Fills the circle centered at p0 with the given radius.
    FillCircle,

    /// Fills the rectangle at p0 with size p1 with a gradient between its
    /// corner colors. `DrawList::getGradient()` returns the colors.
    FillRectGradient,

    /// Draws the connected line segments p0 to p1 to p2, thickness pixels wide.
    Polyline,
};

/// Holds the corner colors of a FillRectGradient command.
struct RectGradient final {
    Color topLeft = Color{255, 255, 255, 255};
    Color topRight = Color{255, 255, 255, 255};
    Color bottomRight = Color{255, 255, 255, 255};
    Color bottomLeft = Color{255, 255, 255, 255};
};

/// Describes one shape or text run to draw.
///
/// Coordinates are logical pixels with the origin at the top-left corner of
/// the viewport and the y axis pointing down.
struct DrawCommand final {
    Vector2 p0 = Vector2{0.0f, 0.0f};
    Vector2 p1 = Vector2{0.0f, 0.0f};
    Vector2 p2 = Vector2{0.0f, 0.0f};
    Color color = Color{255, 255, 255, 255};

    /// The stroke or line width.
    f32 thickness = 0.0f;

    /// The corner radius of FillRect and StrokeRect, or the radius of FillCircle.
    f32 radius = 0.0f;

    /// The font size of a Text command.
    f32 fontSize = 0.0f;

    /// The range of a Text command's UTF-8 bytes in the draw list's text buffer.
    u32 textOffset = 0;
    u32 textLength = 0;

    /// The index of a FillRectGradient command's colors in the draw list.
    u32 gradientIndex = 0;

    /// The index of the clip rectangle in `DrawList::getClipRects()`.
    u16 clipIndex = 0;

    DrawCommandKind kind = DrawCommandKind::FillRect;
};

/// Describes a run of commands that share one depth.
///
/// Each window draws into its own ranges. `DrawList::getRanges()` returns them
/// sorted back to front, so drawing the ranges in order stacks windows
/// correctly.
struct DrawRange final {
    u32 commandBegin = 0;
    u32 commandEnd = 0;
    u64 depth = 0;
};

/// Holds the output of one devgui frame: shapes and text in drawing order.
///
/// A renderer reads the list after the frame ends. The list owns copies of its
/// text, so labels built in temporary buffers stay valid. `clear()` keeps the
/// allocated capacity, so a UI whose size does not grow allocates nothing per
/// frame once warmed up.
class DrawList final {
public:
    DrawList() noexcept;

    /// Removes all commands and sets the viewport size for the next frame.
    void
    reset(const Vector2& viewportSize) noexcept;

    /// Adds a clip rectangle and returns its index.
    [[nodiscard]] u16
    addClipRect(const F32Rect2D& rect);

    /// Appends a command. For a Text command, pass its UTF-8 text.
    void
    addCommand(const DrawCommand& command, std::string_view text = {});

    /// Appends a FillRectGradient command with its corner colors.
    void
    addCommand(const DrawCommand& command, const RectGradient& gradient);

    /// Starts a new range at the given depth for the commands added next.
    void
    beginRange(u64 depth);

    /// Closes the open range and sorts all ranges back to front.
    ///
    /// Ranges with equal depth keep the order in which they were added.
    void
    finish() noexcept;

    /// Returns the viewport size in logical pixels.
    [[nodiscard]] Vector2
    getViewportSize() const noexcept;

    [[nodiscard]] std::span<const DrawCommand>
    getCommands() const noexcept;

    [[nodiscard]] std::span<const F32Rect2D>
    getClipRects() const noexcept;

    /// Returns the ranges of commands sorted back to front.
    [[nodiscard]] std::span<const DrawRange>
    getRanges() const noexcept;

    /// Returns the text of a Text command.
    [[nodiscard]] std::string_view
    getText(const DrawCommand& command) const noexcept;

    /// Returns the corner colors of a FillRectGradient command.
    [[nodiscard]] const RectGradient&
    getGradient(const DrawCommand& command) const noexcept;

private:
    std::vector<DrawCommand> commands_;
    std::vector<F32Rect2D> clipRects_;
    std::vector<DrawRange> ranges_;
    std::vector<RectGradient> gradients_;
    std::string text_;
    Vector2 viewportSize_ = Vector2{0.0f, 0.0f};
};

} // namespace pomdog::devgui
