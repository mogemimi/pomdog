// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <X11/X.h>

namespace Pomdog {
namespace Detail {
namespace X11 {

struct X11AtomCache {
    Atom WmDeleteWindow = None;
    Atom Utf8String = None;
    Atom NetWmName = None;
    Atom NetWmIconName = None;
};

} // namespace X11
} // namespace Detail
} // namespace Pomdog
