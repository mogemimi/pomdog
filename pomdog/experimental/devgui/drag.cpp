// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/drag.h"
#include "pomdog/experimental/devgui/input_text.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <string>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
namespace {

/// The factor applied to the drag speed while shift is held.
constexpr f32 fineDragFactor = 0.1f;

/// How far a mouse travels after a press before the press drags the value. A
/// release before that is a click, which edits the value as text.
constexpr f32 dragStartTravel = 3.0f;

/// Returns how far the pointer travels after a press before the press drags
/// the value.
[[nodiscard]] f32
getDragStartTravel(const UI& ui) noexcept
{
    return (ui.getInput().pointerType == PointerType::Touch) ? detail::touchSlop : dragStartTravel;
}

/// Returns the value change for this frame's pointer movement, or zero when
/// the field is not being dragged.
[[nodiscard]] f32
dragDelta(UI& ui, const Response& response, f32 speed) noexcept
{
    // NOTE: Ignore the movement on the frame the drag starts; it happened
    // before the press.
    if (!response.isHeld() || response.activated || (ui.getActiveTravel() < getDragStartTravel(ui))) {
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

/// Returns the number that text spells, or nullopt when it spells none.
template <typename T>
[[nodiscard]] std::optional<T>
parseNumber(std::string_view text) noexcept
{
    if constexpr (std::is_same_v<T, f32>) {
        return detail::parseFloat(text);
    }
    else {
        static_assert(std::is_same_v<T, i32>);
        return detail::parseInt(text);
    }
}

/// Shows the field drawn in field as a text field while it edits value as
/// text, and sets value, kept within [minValue, maxValue], when the editing
/// ends. Text that spells no number keeps value.
template <typename T>
[[nodiscard]] Response
editNumberAsText(UI& ui, WidgetID id, const F32Rect2D& field, T& value, T minValue, T maxValue, textinput::TextInputPurpose purpose)
{
    std::string committed;
    auto response = detail::editTextInPlace(ui, id, field, InputTextOptions{.placeholder = {}, .purpose = purpose}, committed);
    const bool textChanged = response.changed;
    response.changed = false;
    if (textChanged) {
        if (const auto parsed = parseNumber<T>(committed); parsed) {
            const auto newValue = std::clamp(*parsed, minValue, maxValue);
            if (newValue != value) {
                value = newValue;
                response.changed = true;
            }
        }
    }
    return response;
}

/// Starts editing the field as text when the user clicked it without dragging.
/// The response then stays active until the editing ends.
void editOnClick(UI& ui, Response& response, std::string_view text)
{
    if (!response.clicked || (ui.getActiveTravel() >= getDragStartTravel(ui))) {
        return;
    }
    detail::beginTextEdit(ui, response.id, text);
    response.active = true;
    response.deactivated = false;
}

Response dragFloatImpl(UI& ui, Key key, std::string_view label, f32& value, f32 speed, f32 minValue, f32 maxValue)
{
    const auto id = ui.makeID(key);
    const auto row = detail::layoutPropertyRow(ui, label);
    if (detail::isEditingTextInPlace(ui, id)) {
        return editNumberAsText(ui, id, row.control, value, minValue, maxValue, textinput::TextInputPurpose::Number);
    }

    auto response = ui.interact(id, row.control);
    ui.addTextFieldArea(row.control, textinput::TextInputPurpose::Number);
    if (response.hovered || response.active) {
        ui.setMouseCursor(MouseCursor::ResizeHorizontal);
    }

    if (const auto delta = dragDelta(ui, response, speed); delta != 0.0f) {
        const auto newValue = std::clamp(value + delta, minValue, maxValue);
        if (newValue != value) {
            value = newValue;
            response.changed = true;
        }
    }

    // NOTE: The text to edit keeps every digit of value, which the field
    // rounds for display.
    detail::NumberText editBuffer;
    editOnClick(ui, response, detail::formatNumberExactly(editBuffer, value));
    detail::NumberText displayBuffer;
    drawField(ui, response, row.control, detail::formatNumber(displayBuffer, value, 3));
    return response;
}

Response dragIntImpl(UI& ui, Key key, std::string_view label, i32& value, f32 speed, i32 minValue, i32 maxValue)
{
    const auto id = ui.makeID(key);
    const auto row = detail::layoutPropertyRow(ui, label);
    if (detail::isEditingTextInPlace(ui, id)) {
        return editNumberAsText(ui, id, row.control, value, minValue, maxValue, textinput::TextInputPurpose::Integer);
    }

    auto response = ui.interact(id, row.control);
    ui.addTextFieldArea(row.control, textinput::TextInputPurpose::Integer);
    if (response.hovered || response.active) {
        ui.setMouseCursor(MouseCursor::ResizeHorizontal);
    }

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
    const auto text = detail::formatNumber(buffer, value);
    editOnClick(ui, response, text);
    drawField(ui, response, row.control, text);
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
