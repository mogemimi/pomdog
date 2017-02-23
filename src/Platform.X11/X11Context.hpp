// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "X11AtomCache.hpp"
#include <X11/Xlib.h>

namespace Pomdog {
namespace Detail {
namespace X11 {

class X11Context {
public:
    X11Context();
    ~X11Context();

    X11AtomCache Atoms;
    ::Display* Display;
};

} // namespace X11
} // namespace Detail
} // namespace Pomdog
