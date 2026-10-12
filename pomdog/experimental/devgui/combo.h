// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/devgui/key.h"
#include "pomdog/experimental/devgui/response.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Displays a labeled drop-down list that selects one of items.
///
/// selectedIndex indexes items; an index outside items shows an empty box.
/// Clicking the box opens the list in a popup, where clicking an item sets
/// selectedIndex, closes the list, and sets `Response::changed`. A long list
/// scrolls. Item text is display text only; items are identified by index.
Response
combo(UI& ui, Key key, std::string_view label, i32& selectedIndex, std::span<const std::string_view> items);

} // namespace pomdog::devgui
