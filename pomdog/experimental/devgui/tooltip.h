// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/i18n/line_break_language.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
struct Response;
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Shows text in a tooltip once the pointer has rested on the widget that
/// response describes for `Style::tooltipDelay`.
///
/// Call it right after the widget, with the widget's response. The text wraps
/// at `Style::tooltipMaxWidth` following the line breaking rules of language,
/// so Japanese, Chinese, Korean, and Thai text breaks where those languages
/// allow. The wrapped text is cached, so a tooltip that stays on screen is
/// wrapped once. While waiting for the delay, the tooltip asks for a frame at
/// the moment it should appear instead of keeping the UI busy.
void tooltip(
    UI& ui,
    const Response& response,
    std::string_view text,
    i18n::LineBreakLanguage language = i18n::LineBreakLanguage::Default);

} // namespace pomdog::devgui
