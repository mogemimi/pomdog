// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/devgui/key.h"
#include "pomdog/experimental/devgui/response.h"
#include "pomdog/math/color.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Displays a labeled color swatch and its hexadecimal value.
///
/// Clicking the swatch opens a color picker in a popup: a saturation and value
/// square, a hue bar, an alpha bar, and a slider for each RGBA channel.
/// `Response::changed` reports frames where value changed. The picker keeps
/// its hue while the color is gray or black, where RGB alone loses it.
Response
colorEdit(UI& ui, Key key, std::string_view label, Color& value);

} // namespace pomdog::devgui
