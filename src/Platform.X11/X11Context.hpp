// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_X11CONTEXT_9A6DB993_HPP
#define POMDOG_X11CONTEXT_9A6DB993_HPP

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

#endif // POMDOG_X11CONTEXT_9A6DB993_HPP
