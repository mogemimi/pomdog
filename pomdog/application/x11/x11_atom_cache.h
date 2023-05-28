// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include <X11/X.h>

namespace pomdog::detail::x11 {

struct X11AtomCache final {
    Atom WmDeleteWindow = None;
    Atom Utf8String = None;
    Atom NetWmName = None;
    Atom NetWmIconName = None;
};

} // namespace pomdog::detail::x11
