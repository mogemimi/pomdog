// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/color_edit.h"
#include "pomdog/experimental/devgui/draw_list.h"
#include "pomdog/experimental/devgui/slider.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
namespace {

using namespace pomdog::devgui::literals;

constexpr Key popupKey = "devgui.color.popup"_key;
constexpr Key hsvKey = "devgui.color.hsv"_key;
constexpr Key saturationValueKey = "devgui.color.sv"_key;
constexpr Key hueKey = "devgui.color.hue"_key;
constexpr Key alphaKey = "devgui.color.alpha"_key;

constexpr f32 pickerWidth = 260.0f;
constexpr f32 saturationValueHeight = 160.0f;
constexpr f32 hueBarWidth = 18.0f;
constexpr f32 alphaBarHeight = 14.0f;
constexpr f32 checkerCellSize = 6.0f;

/// Holds a color as hue, saturation, and value, each in [0, 1].
struct HSV final {
    f32 h = 0.0f;
    f32 s = 0.0f;
    f32 v = 0.0f;
};

[[nodiscard]] HSV
toHSV(const Color& color) noexcept
{
    const auto r = static_cast<f32>(color.r) / 255.0f;
    const auto g = static_cast<f32>(color.g) / 255.0f;
    const auto b = static_cast<f32>(color.b) / 255.0f;
    const auto maxValue = std::max({r, g, b});
    const auto minValue = std::min({r, g, b});
    const auto delta = maxValue - minValue;

    HSV hsv;
    hsv.v = maxValue;
    hsv.s = (maxValue > 0.0f) ? (delta / maxValue) : 0.0f;
    if (delta > 0.0f) {
        auto h = 0.0f;
        if (maxValue == r) {
            h = (g - b) / delta;
            if (h < 0.0f) {
                h += 6.0f;
            }
        }
        else if (maxValue == g) {
            h = (b - r) / delta + 2.0f;
        }
        else {
            h = (r - g) / delta + 4.0f;
        }
        hsv.h = h / 6.0f;
    }
    return hsv;
}

[[nodiscard]] u8
toChannel(f32 value) noexcept
{
    return static_cast<u8>(std::lround(std::clamp(value, 0.0f, 1.0f) * 255.0f));
}

[[nodiscard]] Color
toColor(const HSV& hsv, u8 alpha) noexcept
{
    const auto h6 = std::fmod(hsv.h, 1.0f) * 6.0f;
    const auto sector = static_cast<i32>(std::floor(h6));
    const auto f = h6 - static_cast<f32>(sector);
    const auto v = hsv.v;
    const auto p = v * (1.0f - hsv.s);
    const auto q = v * (1.0f - hsv.s * f);
    const auto t = v * (1.0f - hsv.s * (1.0f - f));

    auto r = v;
    auto g = t;
    auto b = p;
    switch (sector) {
    case 1:
        r = q;
        g = v;
        b = p;
        break;
    case 2:
        r = p;
        g = v;
        b = t;
        break;
    case 3:
        r = p;
        g = q;
        b = v;
        break;
    case 4:
        r = t;
        g = p;
        b = v;
        break;
    case 5:
        r = v;
        g = p;
        b = q;
        break;
    default:
        break;
    }
    return Color{toChannel(r), toChannel(g), toChannel(b), alpha};
}

[[nodiscard]] bool
equalRGB(const Color& a, const Color& b) noexcept
{
    return (a.r == b.r) && (a.g == b.g) && (a.b == b.b);
}

// NOTE: Store hue, saturation, and value as 21-bit fixed-point numbers, which
// fit in one stored value with more precision than the 8-bit channels.
constexpr u64 fixedMax = (u64(1) << 21) - 1;

[[nodiscard]] u64
toFixed(f32 value) noexcept
{
    return static_cast<u64>(std::llround(std::clamp(value, 0.0f, 1.0f) * static_cast<f32>(fixedMax)));
}

[[nodiscard]] f32
fromFixed(u64 value) noexcept
{
    return static_cast<f32>(value & fixedMax) / static_cast<f32>(fixedMax);
}

[[nodiscard]] u64
packHSV(const HSV& hsv) noexcept
{
    return (toFixed(hsv.h) << 42) | (toFixed(hsv.s) << 21) | toFixed(hsv.v);
}

[[nodiscard]] HSV
unpackHSV(u64 value) noexcept
{
    return HSV{fromFixed(value >> 42), fromFixed(value >> 21), fromFixed(value)};
}

[[nodiscard]] Color
withAlpha(Color color, u8 alpha) noexcept
{
    color.a = alpha;
    return color;
}

/// Draws a light and dark checkerboard that shows through translucent colors.
void drawCheckerboard(UI& ui, const F32Rect2D& rect)
{
    ui.fillRect(rect, Color{204, 204, 204, 255});
    const auto dark = Color{153, 153, 153, 255};
    i32 row = 0;
    for (auto y = rect.y; y < rect.y + rect.height; y += checkerCellSize, ++row) {
        i32 column = 0;
        for (auto x = rect.x; x < rect.x + rect.width; x += checkerCellSize, ++column) {
            if (((row + column) % 2) == 1) {
                const auto w = std::min(checkerCellSize, rect.x + rect.width - x);
                const auto h = std::min(checkerCellSize, rect.y + rect.height - y);
                ui.fillRect(F32Rect2D{x, y, w, h}, dark);
            }
        }
    }
}

/// Draws a thin marker band with a dark outline, used on the hue and alpha bars.
void drawBarMarker(UI& ui, const F32Rect2D& rect)
{
    detail::drawFrame(ui, rect, Color{255, 255, 255, 255}, Color{0, 0, 0, 160}, 1.0f);
}

[[nodiscard]] f32
fractionIn(f32 position, f32 start, f32 length) noexcept
{
    if (length <= 0.0f) {
        return 0.0f;
    }
    return std::clamp((position - start) / length, 0.0f, 1.0f);
}

} // namespace

Response colorEdit(UI& ui, Key key, std::string_view label, Color& value)
{
    const auto& style = ui.getStyle();
    const auto id = ui.makeID(key);
    const auto row = detail::layoutPropertyRow(ui, label);
    const auto& control = row.control;
    auto response = ui.interact(id, control);

    const auto popupID = combineID(id, popupKey);
    bool open = ui.isPopupOpen(popupID);
    if (response.clicked && !open) {
        ui.openPopup(popupID);
        open = true;
    }

    // NOTE: The swatch shows translucent colors over a checkerboard.
    const auto swatch = F32Rect2D{control.x, control.y, std::min(control.width, std::round(style.getFrameHeight() * 2.0f)), control.height};
    const auto border = detail::getInputBorderColor(style, response, open);
    ui.fillRect(swatch, border, style.frameRounding);
    const auto swatchInner = F32Rect2D{swatch.x + 1.0f, swatch.y + 1.0f, swatch.width - 2.0f, swatch.height - 2.0f};
    if (value.a < 255) {
        drawCheckerboard(ui, swatchInner);
    }
    ui.fillRect(swatchInner, value, std::max(0.0f, style.frameRounding - 1.0f));

    detail::NumberText hexBuffer;
    const auto hexResult = fmt::format_to_n(
        hexBuffer.data(),
        hexBuffer.size(),
        "#{:02X}{:02X}{:02X}{:02X}",
        static_cast<u32>(value.r),
        static_cast<u32>(value.g),
        static_cast<u32>(value.b),
        static_cast<u32>(value.a));
    const auto hexText = std::string_view{hexBuffer.data(), std::min(hexResult.size, hexBuffer.size())};
    const auto hexX = swatch.x + swatch.width + style.itemSpacing.x;
    ui.drawText(Vector2{hexX, detail::getTextTop(style, control)}, hexText, style.textSecondaryColor, control.x + control.width - hexX);

    if (!open) {
        return response;
    }

    const auto rowHeight = style.getFrameHeight();
    const auto spacing = style.itemSpacing.y;
    const auto pickerHeight = saturationValueHeight + alphaBarHeight + rowHeight * 4.0f + spacing * 5.0f + style.popupPadding.y * 2.0f;
    ui.beginPopup(popupID, swatch, Vector2{pickerWidth, pickerHeight});

    // NOTE: Prefer the stored HSV while it still describes value, so that the
    // hue survives gray and black colors and markers do not snap to 8-bit steps.
    const auto stateID = combineID(id, hsvKey);
    auto hsv = toHSV(value);
    if (const auto stored = ui.loadState(stateID); stored) {
        const auto candidate = unpackHSV(*stored);
        if (equalRGB(toColor(candidate, value.a), value)) {
            hsv = candidate;
        }
    }

    auto newValue = value;
    bool editedHSV = false;

    const auto area = ui.allocate(0.0f, saturationValueHeight);
    const auto svRect = F32Rect2D{area.x, area.y, std::max(0.0f, area.width - hueBarWidth - style.itemSpacing.x), area.height};
    const auto hueRect = F32Rect2D{svRect.x + svRect.width + style.itemSpacing.x, area.y, hueBarWidth, area.height};

    const auto& pointer = ui.getInput().pointerPosition;
    if (const auto r = ui.interact(ui.makeID(saturationValueKey), svRect); r.isHeld()) {
        hsv.s = fractionIn(pointer.x, svRect.x, svRect.width);
        hsv.v = 1.0f - fractionIn(pointer.y, svRect.y, svRect.height);
        editedHSV = true;
    }
    if (const auto r = ui.interact(ui.makeID(hueKey), hueRect); r.isHeld()) {
        hsv.h = fractionIn(pointer.y, hueRect.y, hueRect.height);
        editedHSV = true;
    }

    const auto alphaRect = ui.allocate(0.0f, alphaBarHeight);
    auto alpha = value.a;
    if (const auto r = ui.interact(ui.makeID(alphaKey), alphaRect); r.isHeld()) {
        alpha = toChannel(fractionIn(pointer.x, alphaRect.x, alphaRect.width));
        editedHSV = true;
    }
    if (editedHSV) {
        newValue = toColor(hsv, alpha);
    }

    // NOTE: The saturation and value square is white to the hue across, then
    // fades to black downward.
    const auto hueColor = toColor(HSV{hsv.h, 1.0f, 1.0f}, 255);
    const auto white = Color{255, 255, 255, 255};
    const auto black = Color{0, 0, 0, 255};
    const auto clear = Color{0, 0, 0, 0};
    ui.fillRectGradient(svRect, RectGradient{white, hueColor, hueColor, white});
    ui.fillRectGradient(svRect, RectGradient{clear, clear, black, black});
    ui.strokeRect(svRect, style.inputBorderColor, 1.0f);
    const auto svMarker = Vector2{svRect.x + hsv.s * svRect.width, svRect.y + (1.0f - hsv.v) * svRect.height};
    ui.fillCircle(svMarker, 6.0f, white);
    ui.fillCircle(svMarker, 4.0f, toColor(hsv, 255));

    // NOTE: The hue bar runs through the six hue sectors from top to bottom.
    for (i32 i = 0; i < 6; ++i) {
        const auto top = hueRect.y + hueRect.height * static_cast<f32>(i) / 6.0f;
        const auto bottom = hueRect.y + hueRect.height * static_cast<f32>(i + 1) / 6.0f;
        const auto topColor = toColor(HSV{static_cast<f32>(i) / 6.0f, 1.0f, 1.0f}, 255);
        const auto bottomColor = toColor(HSV{static_cast<f32>(i + 1) / 6.0f, 1.0f, 1.0f}, 255);
        ui.fillRectGradient(F32Rect2D{hueRect.x, top, hueRect.width, bottom - top}, RectGradient{topColor, topColor, bottomColor, bottomColor});
    }
    ui.strokeRect(hueRect, style.inputBorderColor, 1.0f);
    const auto hueY = hueRect.y + hsv.h * hueRect.height;
    drawBarMarker(ui, F32Rect2D{hueRect.x - 2.0f, std::round(hueY) - 2.0f, hueRect.width + 4.0f, 4.0f});

    drawCheckerboard(ui, alphaRect);
    const auto opaque = withAlpha(newValue, 255);
    const auto transparent = withAlpha(newValue, 0);
    ui.fillRectGradient(alphaRect, RectGradient{transparent, opaque, opaque, transparent});
    ui.strokeRect(alphaRect, style.inputBorderColor, 1.0f);
    const auto alphaX = alphaRect.x + alphaRect.width * static_cast<f32>(alpha) / 255.0f;
    drawBarMarker(ui, F32Rect2D{std::round(alphaX) - 2.0f, alphaRect.y - 2.0f, 4.0f, alphaRect.height + 4.0f});

    std::array<i32, 4> channels = {newValue.r, newValue.g, newValue.b, newValue.a};
    const std::array<Key, 4> channelKeys = {"r"_key, "g"_key, "b"_key, "a"_key};
    const std::array<std::string_view, 4> channelLabels = {"R", "G", "B", "A"};
    bool editedChannel = false;
    for (std::size_t i = 0; i < channels.size(); ++i) {
        if (sliderInt(ui, channelKeys[i], channelLabels[i], channels[i], 0, 255).changed) {
            editedChannel = true;
        }
    }
    if (editedChannel) {
        newValue = Color{
            static_cast<u8>(channels[0]),
            static_cast<u8>(channels[1]),
            static_cast<u8>(channels[2]),
            static_cast<u8>(channels[3]),
        };
        const auto derived = toHSV(newValue);
        hsv.v = derived.v;
        if (derived.v > 0.0f) {
            hsv.s = derived.s;
        }
        if (derived.s > 0.0f) {
            hsv.h = derived.h;
        }
    }

    ui.storeState(stateID, packHSV(hsv));
    ui.endPopup();

    if (newValue != value) {
        value = newValue;
        response.changed = true;

        // NOTE: The swatch and the square were drawn before the edit. Ask for
        // another frame so that they catch up even if no input follows.
        ui.requestUpdateAt(ui.getInput().timeSeconds);
    }
    return response;
}

} // namespace pomdog::devgui
