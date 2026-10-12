// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/devgui/key.h"
#include "pomdog/experimental/devgui/response.h"
#include "pomdog/math/f32_rect2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Displays a button sized to its label. `Response::clicked` reports a click.
[[nodiscard]] Response
button(UI& ui, Key key, std::string_view label);

/// Displays a small button with a cross in rect, outside the layout, for
/// example at the end of a list row. `Response::clicked` reports a click.
///
/// Place it over a widget that takes its press with `UI::interactBehind()`,
/// such as a list view row, so that the press reaches the button.
Response
closeButton(UI& ui, Key key, const F32Rect2D& rect);

} // namespace pomdog::devgui
