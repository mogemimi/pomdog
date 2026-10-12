// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/horizontal_scope.h"
#include "pomdog/experimental/devgui/ui.h"

namespace pomdog::devgui {

HorizontalScope::HorizontalScope(UI& ui)
    : ui_(ui)
{
    ui_.beginHorizontal();
}

HorizontalScope::~HorizontalScope()
{
    ui_.endHorizontal();
}

HorizontalScope horizontal(UI& ui)
{
    return HorizontalScope{ui};
}

} // namespace pomdog::devgui
