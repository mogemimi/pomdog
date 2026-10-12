// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/id_scope.h"
#include "pomdog/experimental/devgui/ui.h"

namespace pomdog::devgui {

IDScope::IDScope(UI& ui, Key key)
    : ui_(ui)
{
    ui_.pushIDScope(key);
}

IDScope::~IDScope()
{
    ui_.popIDScope();
}

IDScope pushID(UI& ui, Key key)
{
    return IDScope{ui, key};
}

} // namespace pomdog::devgui
