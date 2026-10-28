// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/widget_helpers.h"
#include "pomdog/experimental/devgui/response.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <system_error>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui::detail {
namespace {

/// The powers of ten that f64 holds exactly, from 1e0 to 1e22. Each product
/// below is exact.
constexpr auto exactPowersOfTen = [] {
    std::array<f64, 23> powers = {};
    f64 power = 1.0;
    for (auto& p : powers) {
        p = power;
        power *= 10.0;
    }
    return powers;
}();

/// The most significant digits that a decimal mantissa keeps, which fit in
/// u64 and are far more than f32 needs.
constexpr i32 maxMantissaDigits = 19;

[[nodiscard]] constexpr bool
isDigit(char c) noexcept
{
    return (c >= '0') && (c <= '9');
}

[[nodiscard]] std::string_view
trimSpaces(std::string_view text) noexcept
{
    const auto first = text.find_first_not_of(" \t");
    if (first == std::string_view::npos) {
        return {};
    }
    const auto last = text.find_last_not_of(" \t");
    return text.substr(first, last - first + 1);
}

} // namespace

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

std::string_view formatNumberExactly(NumberText& buffer, f32 value) noexcept
{
    // NOTE: fmt writes the shortest text that reads back as the same value.
    const auto result = fmt::format_to_n(buffer.data(), buffer.size(), "{}", value);
    return std::string_view{buffer.data(), std::min(result.size, buffer.size())};
}

std::optional<f32> parseFloat(std::string_view text) noexcept
{
    // NOTE: The C library and iostreams read numbers in the global locale,
    // where the decimal point may be ",", so this reads the digits itself.
    text = trimSpaces(text);
    std::size_t i = 0;
    bool negative = false;
    if ((i < text.size()) && ((text[i] == '+') || (text[i] == '-'))) {
        negative = (text[i] == '-');
        ++i;
    }

    // NOTE: The value is mantissa times ten to the power of exponent. Digits
    // past the ones that mantissa keeps only scale it.
    u64 mantissa = 0;
    i32 exponent = 0;
    i32 keptDigits = 0;
    bool hasDigits = false;
    const auto readDigit = [&](char c, bool fraction) {
        hasDigits = true;
        if (keptDigits < maxMantissaDigits) {
            mantissa = mantissa * 10 + static_cast<u64>(c - '0');
            if (mantissa != 0) {
                ++keptDigits;
            }
            if (fraction) {
                --exponent;
            }
        }
        else if (!fraction) {
            ++exponent;
        }
    };
    for (; (i < text.size()) && isDigit(text[i]); ++i) {
        readDigit(text[i], false);
    }
    if ((i < text.size()) && (text[i] == '.')) {
        ++i;
        for (; (i < text.size()) && isDigit(text[i]); ++i) {
            readDigit(text[i], true);
        }
    }
    if (!hasDigits) {
        return std::nullopt;
    }

    if ((i < text.size()) && ((text[i] == 'e') || (text[i] == 'E'))) {
        ++i;
        bool negativeExponent = false;
        if ((i < text.size()) && ((text[i] == '+') || (text[i] == '-'))) {
            negativeExponent = (text[i] == '-');
            ++i;
        }
        if ((i >= text.size()) || !isDigit(text[i])) {
            return std::nullopt;
        }
        i32 written = 0;
        for (; (i < text.size()) && isDigit(text[i]); ++i) {
            written = std::min(written * 10 + (text[i] - '0'), 9999);
        }
        exponent += negativeExponent ? -written : written;
    }
    if (i != text.size()) {
        return std::nullopt;
    }

    // NOTE: A mantissa and a power of ten that f64 holds exactly make one
    // correctly rounded product. Other values round a few more times in f64,
    // which is still far finer than f32. Beyond 1e400 and 1e-400, f32 holds
    // only infinity and zero.
    auto value = static_cast<f64>(mantissa);
    if (mantissa != 0) {
        exponent = std::clamp(exponent, -400, 400);
        for (; exponent > 22; exponent -= 22) {
            value *= exactPowersOfTen[22];
        }
        for (; exponent < -22; exponent += 22) {
            value /= exactPowersOfTen[22];
        }
        if (exponent >= 0) {
            value *= exactPowersOfTen[static_cast<std::size_t>(exponent)];
        }
        else {
            value /= exactPowersOfTen[static_cast<std::size_t>(-exponent)];
        }
    }

    // NOTE: Values from halfway between the largest f32 and the next power of
    // two round to infinity.
    if (value >= 0x1.ffffffp127) {
        return std::nullopt;
    }
    const auto result = static_cast<f32>(value);
    return negative ? -result : result;
}

std::optional<i32> parseInt(std::string_view text) noexcept
{
    text = trimSpaces(text);

    // NOTE: std::from_chars reads integers the same way in every locale, but
    // without a plus sign.
    if ((text.size() >= 2) && (text[0] == '+') && isDigit(text[1])) {
        text.remove_prefix(1);
    }
    i32 value = 0;
    const auto [end, ec] = std::from_chars(text.data(), text.data() + text.size(), value);
    if ((ec != std::errc{}) || (end != text.data() + text.size())) {
        return std::nullopt;
    }
    return value;
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
