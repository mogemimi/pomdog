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

/// Displays a labeled slider that sets value within [minValue, maxValue].
///
/// Pressing the slider jumps value to the pointer, and dragging follows it.
/// value is displayed with three decimal places.
Response
sliderFloat(UI& ui, Key key, std::string_view label, f32& value, f32 minValue, f32 maxValue);

/// Displays a labeled slider that sets value within [minValue, maxValue].
Response
sliderInt(UI& ui, Key key, std::string_view label, i32& value, i32 minValue, i32 maxValue);

} // namespace pomdog::devgui
