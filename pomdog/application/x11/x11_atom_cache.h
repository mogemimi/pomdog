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
    // NOTE: _NET_WM_STATE and related atoms for fullscreen/maximize support.
    Atom NetWmState = None;
    Atom NetWmStateFullscreen = None;
    Atom NetWmStateMaximizedVert = None;
    Atom NetWmStateMaximizedHorz = None;
    // NOTE: _MOTIF_WM_HINTS for borderless windowed mode (decoration removal).
    Atom MotifWmHints = None;
};

} // namespace pomdog::detail::x11
