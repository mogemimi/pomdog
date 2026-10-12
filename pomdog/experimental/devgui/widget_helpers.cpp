// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/widget_helpers.h"
#include "pomdog/experimental/devgui/response.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui::detail {

PropertyRow layoutPropertyRow(UI& ui, std::string_view label, f32 height)
{
    const auto& style = ui.getStyle();

    PropertyRow result;
    result.row = ui.allocate(0.0f, height);
    result.control = result.row;
    if (label.empty()) {
        return result;
    }

    const auto labelWidth = std::floor(result.row.width * style.labelColumnRatio);
    const auto labelTop = std::round(result.row.y + (style.getFrameHeight() - style.fontSize) * 0.5f);
    ui.drawText(Vector2{result.row.x, labelTop}, label, style.textColor, labelWidth - style.itemSpacing.x);
    result.control.x += labelWidth;
    result.control.width = std::max(0.0f, result.control.width - labelWidth);
    return result;
}

PropertyRow layoutPropertyRow(UI& ui, std::string_view label)
{
    return layoutPropertyRow(ui, label, ui.getStyle().getFrameHeight());
}

std::string_view formatNumber(NumberText& buffer, f32 value, i32 decimals) noexcept
{
    const auto result = fmt::format_to_n(buffer.data(), buffer.size(), "{:.{}f}", value, std::clamp(decimals, 0, 9));
    return std::string_view{buffer.data(), std::min(result.size, buffer.size())};
}

std::string_view formatNumber(NumberText& buffer, i32 value) noexcept
{
    const auto result = fmt::format_to_n(buffer.data(), buffer.size(), "{}", value);
    return std::string_view{buffer.data(), std::min(result.size, buffer.size())};
}

f32 getTextTop(const Style& style, const F32Rect2D& rect) noexcept
{
    return std::round(rect.y + (rect.height - style.fontSize) * 0.5f);
}

void drawCenteredText(UI& ui, const F32Rect2D& rect, std::string_view text, const Color& color)
{
    const auto& style = ui.getStyle();
    const auto width = ui.measureText(text);
    const auto x = rect.x + std::max(style.framePadding.x, std::round((rect.width - width) * 0.5f));
    ui.drawText(Vector2{x, getTextTop(style, rect)}, text, color);
}

void drawFrame(UI& ui, const F32Rect2D& rect, const Color& fill, const Color& border, f32 radius)
{
    // NOTE: Two fills draw a crisp border without antialiasing: the outer one
    // in the border color, the inner one inset by a pixel.
    ui.fillRect(rect, border, radius);
    const auto inner = F32Rect2D{rect.x + 1.0f, rect.y + 1.0f, std::max(0.0f, rect.width - 2.0f), std::max(0.0f, rect.height - 2.0f)};
    ui.fillRect(inner, fill, std::max(0.0f, radius - 1.0f));
}

Color getInputBorderColor(const Style& style, const Response& response, bool focused) noexcept
{
    if (focused || response.active) {
        return style.accentColor;
    }
    if (response.hovered) {
        return style.inputHoveredBorderColor;
    }
    return style.inputBorderColor;
}

void drawChevron(UI& ui, const Vector2& center, f32 size, ChevronDirection direction, const Color& color)
{
    constexpr f32 thickness = 1.5f;
    switch (direction) {
    case ChevronDirection::Right:
        ui.drawPolyline(Vector2{center.x - size * 0.5f, center.y - size}, Vector2{center.x + size * 0.5f, center.y}, Vector2{center.x - size * 0.5f, center.y + size}, color, thickness);
        break;
    case ChevronDirection::Down:
        ui.drawPolyline(Vector2{center.x - size, center.y - size * 0.5f}, Vector2{center.x, center.y + size * 0.5f}, Vector2{center.x + size, center.y - size * 0.5f}, color, thickness);
        break;
    }
}

void drawCloseButton(UI& ui, const F32Rect2D& rect, const Response& response, bool visible)
{
    const auto& style = ui.getStyle();
    const bool hot = response.hovered || response.active;
    if (hot) {
        // NOTE: Inset the highlight so that it does not touch the edges of a
        // title bar or a tab.
        const auto inset = std::min(3.0f, std::min(rect.width, rect.height) * 0.15f);
        const auto highlight = F32Rect2D{rect.x + inset, rect.y + inset, rect.width - inset * 2.0f, rect.height - inset * 2.0f};
        ui.fillRect(highlight, response.active ? style.inputHoveredBorderColor : style.inputBorderColor, style.frameRounding);
    }
    if (!visible && !hot) {
        return;
    }
    const auto center = Vector2{rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f};
    const auto r = std::round(std::min(rect.width, rect.height) * 0.18f);
    const auto color = hot ? style.textColor : style.textSecondaryColor;
    ui.drawLine(Vector2{center.x - r, center.y - r}, Vector2{center.x + r, center.y + r}, color, 1.25f);
    ui.drawLine(Vector2{center.x - r, center.y + r}, Vector2{center.x + r, center.y - r}, color, 1.25f);
}

void drawDragGhost(UI& ui, const F32Rect2D& rect, std::string_view label, f32 textPadding)
{
    const auto& style = ui.getStyle();
    const auto fade = [](Color color, u8 alpha) {
        color.a = static_cast<u8>((static_cast<u32>(color.a) * alpha) / 255);
        return color;
    };

    // NOTE: The overlay lies above every window and popup and takes no input,
    // so the copy follows the pointer without blocking the drop targets.
    ui.beginOverlay();
    drawFrame(ui, rect, fade(style.popupBackgroundColor, 200), fade(style.popupBorderColor, 200), style.frameRounding);
    const auto textX = rect.x + textPadding;
    ui.drawText(Vector2{textX, getTextTop(style, rect)}, label, fade(style.textColor, 200), std::max(0.0f, rect.x + rect.width - textPadding - textX));
    ui.endOverlay();
}

void drawDropIndicator(UI& ui, const F32Rect2D& extent, f32 position, bool vertical)
{
    const auto& style = ui.getStyle();
    constexpr f32 thickness = 2.0f;
    if (vertical) {
        ui.fillRect(F32Rect2D{position - thickness * 0.5f, extent.y, thickness, extent.height}, style.dropIndicatorColor);
    }
    else {
        ui.fillRect(F32Rect2D{extent.x, position - thickness * 0.5f, extent.width, thickness}, style.dropIndicatorColor);
    }
}

void drawCheckMark(UI& ui, const F32Rect2D& box, const Color& color)
{
    const auto size = std::min(box.width, box.height);
    const auto thickness = std::max(1.5f, size * 0.12f);
    const auto left = Vector2{box.x + size * 0.22f, box.y + size * 0.52f};
    const auto bottom = Vector2{box.x + size * 0.42f, box.y + size * 0.72f};
    const auto right = Vector2{box.x + size * 0.78f, box.y + size * 0.30f};
    ui.drawPolyline(left, bottom, right, color, thickness);
}

} // namespace pomdog::devgui::detail
