// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include <X11/X.h>

namespace Pomdog::Detail::X11 {

struct X11AtomCache {
    Atom WmDeleteWindow = None;
    Atom Utf8String = None;
    Atom NetWmName = None;
    Atom NetWmIconName = None;
};

} // namespace Pomdog::Detail::X11
