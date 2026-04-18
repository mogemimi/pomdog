// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <X11/X.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::x11 {

struct X11AtomCache final {
    Atom WmDeleteWindow = None;
    Atom Utf8String = None;
    Atom NetWmName = None;
    Atom NetWmIconName = None;
};

} // namespace pomdog::detail::x11
