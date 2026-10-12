// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/separator.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {

void separator(UI& ui)
{
    const auto& style = ui.getStyle();
    const auto rect = ui.allocate(0.0f, std::max(1.0f, style.itemSpacing.y * 2.0f));
    const auto y = std::floor(rect.y + rect.height * 0.5f);
    ui.fillRect(F32Rect2D{rect.x, y, rect.width, 1.0f}, style.separatorColor);
}

} // namespace pomdog::devgui
