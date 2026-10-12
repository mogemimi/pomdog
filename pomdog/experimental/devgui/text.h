// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/color.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Displays a single line of text in the style's text color.
///
/// The row is as tall as a framed widget, so text lines up with buttons in a
/// horizontal row.
void text(UI& ui, std::string_view text);

/// Displays a single line of text in color.
void text(UI& ui, std::string_view text, const Color& color);

/// Displays a label and a value in the two columns of a property row.
void labelText(UI& ui, std::string_view label, std::string_view text);

} // namespace pomdog::devgui
