// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/x11/x11_atom_cache.h"
#include "pomdog/utility/errors.h"
#include <X11/Xlib.h>
#include <memory>

namespace pomdog::detail::x11 {

class X11Context final {
public:
    X11AtomCache Atoms;
    ::Display* Display = nullptr;

public:
    X11Context() noexcept;
    ~X11Context() noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    initialize() noexcept;
};

} // namespace pomdog::detail::x11
