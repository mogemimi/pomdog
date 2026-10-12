// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/drag.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
namespace {

/// The factor applied to the drag speed while shift is held.
constexpr f32 fineDragFactor = 0.1f;

/// Returns the value change for this frame's pointer movement, or zero when
/// the field is not being dragged.
[[nodiscard]] f32
dragDelta(UI& ui, const Response& response, f32 speed) noexcept
{
    // NOTE: Ignore the movement on the frame the drag starts; it happened
    // before the press.
    if (!response.isHeld() || response.activated) {
        return 0.0f;
    }
    const auto factor = ui.getInput().shiftDown ? fineDragFactor : 1.0f;
    return ui.getPointerDelta().x * speed * factor;
}

void drawField(UI& ui, const Response& response, const F32Rect2D& control, std::string_view valueText)
{
    const auto& style = ui.getStyle();
    const auto border = detail::getInputBorderColor(style, response, false);
    detail::drawFrame(ui, control, style.inputBackgroundColor, border, style.frameRounding);
    detail::drawCenteredText(ui, control, valueText, style.textColor);
}

Response dragFloatImpl(UI& ui, Key key, std::string_view label, f32& value, f32 speed, f32 minValue, f32 maxValue)
{
    const auto id = ui.makeID(key);
    const auto row = detail::layoutPropertyRow(ui, label);
    auto response = ui.interact(id, row.control);

    if (const auto delta = dragDelta(ui, response, speed); delta != 0.0f) {
        const auto newValue = std::clamp(value + delta, minValue, maxValue);
        if (newValue != value) {
            value = newValue;
            response.changed = true;
        }
    }

    detail::NumberText buffer;
    drawField(ui, response, row.control, detail::formatNumber(buffer, value, 3));
    return response;
}

Response dragIntImpl(UI& ui, Key key, std::string_view label, i32& value, f32 speed, i32 minValue, i32 maxValue)
{
    const auto id = ui.makeID(key);
    const auto row = detail::layoutPropertyRow(ui, label);
    auto response = ui.interact(id, row.control);

    if (const auto delta = dragDelta(ui, response, speed); delta != 0.0f) {
        // NOTE: Keep the fraction of a step in the active scratch value so that
        // slow drags still move the value.
        auto& remainder = ui.getActiveScratch();
        remainder += delta;
        const auto steps = std::trunc(remainder);
        remainder -= steps;

        const auto newValue = static_cast<i32>(std::clamp(
            static_cast<f64>(value) + static_cast<f64>(steps),
            static_cast<f64>(minValue),
            static_cast<f64>(maxValue)));
        if (newValue != value) {
            value = newValue;
            response.changed = true;
        }
    }

    detail::NumberText buffer;
    drawField(ui, response, row.control, detail::formatNumber(buffer, value));
    return response;
}

} // namespace

Response dragFloat(UI& ui, Key key, std::string_view label, f32& value, f32 speed)
{
    constexpr auto limit = std::numeric_limits<f32>::max();
    return dragFloatImpl(ui, key, label, value, speed, -limit, limit);
}

Response dragFloat(UI& ui, Key key, std::string_view label, f32& value, f32 speed, f32 minValue, f32 maxValue)
{
    return dragFloatImpl(ui, key, label, value, speed, minValue, std::max(minValue, maxValue));
}

Response dragInt(UI& ui, Key key, std::string_view label, i32& value, f32 speed)
{
    return dragIntImpl(ui, key, label, value, speed, std::numeric_limits<i32>::min(), std::numeric_limits<i32>::max());
}

Response dragInt(UI& ui, Key key, std::string_view label, i32& value, f32 speed, i32 minValue, i32 maxValue)
{
    return dragIntImpl(ui, key, label, value, speed, minValue, std::max(minValue, maxValue));
}

} // namespace pomdog::devgui
