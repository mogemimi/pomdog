// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/slider.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
namespace {

/// The height of the slider track.
constexpr f32 trackHeight = 4.0f;

/// The radius of the thumb at rest and while hovered or dragged.
constexpr f32 thumbRadius = 6.0f;
constexpr f32 thumbRadiusHot = 7.0f;

/// Holds the areas of a slider row: the track the user drags along and the
/// value shown to its right.
struct SliderLayout final {
    F32Rect2D hitRect = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 trackLeft = 0.0f;
    f32 trackWidth = 0.0f;
    f32 valueRight = 0.0f;
};

[[nodiscard]] SliderLayout
layoutSlider(const Style& style, const F32Rect2D& control) noexcept
{
    // NOTE: Reserve a column for the value, wide enough for numbers such as
    // "-1000.000", so the track keeps its length while the value changes.
    const auto valueWidth = std::round(style.fontSize * 4.0f);

    SliderLayout layout;
    layout.valueRight = control.x + control.width;
    layout.hitRect = control;
    layout.hitRect.width = std::max(0.0f, control.width - valueWidth - style.itemSpacing.x);
    layout.trackLeft = layout.hitRect.x + thumbRadius;
    layout.trackWidth = std::max(0.0f, layout.hitRect.width - thumbRadius * 2.0f);
    return layout;
}

/// Returns the slider position of value as a fraction in [0, 1].
[[nodiscard]] f32
toFraction(f64 value, f64 minValue, f64 maxValue) noexcept
{
    if (maxValue <= minValue) {
        return 0.0f;
    }
    return static_cast<f32>(std::clamp((value - minValue) / (maxValue - minValue), 0.0, 1.0));
}

/// Returns the fraction the pointer selects while the slider is dragged, or a
/// negative number when it is not.
[[nodiscard]] f32
dragFraction(UI& ui, const Response& response, const SliderLayout& layout) noexcept
{
    if (!detail::followsPointer(ui, response) || (layout.trackWidth <= 0.0f)) {
        return -1.0f;
    }
    return std::clamp((ui.getInput().pointerPosition.x - layout.trackLeft) / layout.trackWidth, 0.0f, 1.0f);
}

void drawSlider(UI& ui, const Response& response, const SliderLayout& layout, f32 fraction, std::string_view valueText)
{
    const auto& style = ui.getStyle();
    const auto& hit = layout.hitRect;
    const auto centerY = hit.y + hit.height * 0.5f;
    const auto track = F32Rect2D{layout.trackLeft, std::round(centerY - trackHeight * 0.5f), layout.trackWidth, trackHeight};
    const auto thumbX = layout.trackLeft + layout.trackWidth * fraction;

    ui.fillRect(track, style.sliderTrackColor, trackHeight * 0.5f);
    ui.fillRect(F32Rect2D{track.x, track.y, thumbX - track.x, track.height}, style.accentColor, trackHeight * 0.5f);

    const bool hot = response.hovered || response.active;
    ui.fillCircle(Vector2{thumbX, centerY}, hot ? thumbRadiusHot : thumbRadius, hot ? style.sliderThumbHoveredColor : style.sliderThumbColor);

    const auto textWidth = ui.measureText(valueText);
    ui.drawText(Vector2{layout.valueRight - textWidth, detail::getTextTop(style, hit)}, valueText, style.textSecondaryColor);
}

} // namespace

Response sliderFloat(UI& ui, Key key, std::string_view label, f32& value, f32 minValue, f32 maxValue)
{
    const auto id = ui.makeID(key);
    const auto row = detail::layoutPropertyRow(ui, label);
    const auto layout = layoutSlider(ui.getStyle(), row.control);
    auto response = ui.interact(id, layout.hitRect);

    if (const auto t = dragFraction(ui, response, layout); t >= 0.0f) {
        const auto newValue = minValue + (maxValue - minValue) * t;
        if (newValue != value) {
            value = newValue;
            response.changed = true;
        }
    }

    detail::NumberText buffer;
    drawSlider(ui, response, layout, toFraction(static_cast<f64>(value), static_cast<f64>(minValue), static_cast<f64>(maxValue)), detail::formatNumber(buffer, value, 3));
    return response;
}

Response sliderInt(UI& ui, Key key, std::string_view label, i32& value, i32 minValue, i32 maxValue)
{
    const auto id = ui.makeID(key);
    const auto row = detail::layoutPropertyRow(ui, label);
    const auto layout = layoutSlider(ui.getStyle(), row.control);
    auto response = ui.interact(id, layout.hitRect);

    const auto minF = static_cast<f64>(minValue);
    const auto maxF = static_cast<f64>(maxValue);
    if (const auto t = dragFraction(ui, response, layout); t >= 0.0f) {
        const auto rounded = std::round(minF + (maxF - minF) * static_cast<f64>(t));
        const auto newValue = static_cast<i32>(std::clamp(rounded, minF, std::max(minF, maxF)));
        if (newValue != value) {
            value = newValue;
            response.changed = true;
        }
    }

    detail::NumberText buffer;
    drawSlider(ui, response, layout, toFraction(static_cast<f64>(value), minF, maxF), detail::formatNumber(buffer, value));
    return response;
}

} // namespace pomdog::devgui
