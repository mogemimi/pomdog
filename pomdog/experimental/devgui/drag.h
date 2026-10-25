// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/devgui/key.h"
#include "pomdog/experimental/devgui/response.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Displays a labeled number field that changes value by dragging sideways.
///
/// value changes by speed per pixel of horizontal pointer movement, or a tenth
/// of that while shift is held. value has no bounds.
///
/// A click without dragging edits value as text, all of it selected, like
/// `inputText()`. Enter or a press elsewhere sets value from the text, and
/// Escape keeps value; text that is not a number also keeps it. The response
/// stays active from the press until the editing ends, and reports changed on
/// the frame that sets value.
Response
dragFloat(UI& ui, Key key, std::string_view label, f32& value, f32 speed);

/// Displays a labeled number field like `dragFloat()` that keeps value within
/// [minValue, maxValue].
Response
dragFloat(UI& ui, Key key, std::string_view label, f32& value, f32 speed, f32 minValue, f32 maxValue);

/// Displays a labeled integer field that changes value by dragging sideways.
///
/// value changes by speed per pixel; fractions carry over while dragging. A
/// click edits value as text, as in `dragFloat()`.
Response
dragInt(UI& ui, Key key, std::string_view label, i32& value, f32 speed);

/// Displays a labeled integer field like `dragInt()` that keeps value within
/// [minValue, maxValue].
Response
dragInt(UI& ui, Key key, std::string_view label, i32& value, f32 speed, i32 minValue, i32 maxValue);

} // namespace pomdog::devgui
