// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "X11AtomCache.hpp"
#include <X11/Xlib.h>

namespace Pomdog::Detail::X11 {

class X11Context {
public:
    X11Context();
    ~X11Context();

    X11AtomCache Atoms;
    ::Display* Display;
};

} // namespace Pomdog::Detail::X11
