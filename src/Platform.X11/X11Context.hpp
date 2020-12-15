// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "X11AtomCache.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <X11/Xlib.h>
#include <memory>

namespace Pomdog::Detail::X11 {

class X11Context final {
public:
    X11AtomCache Atoms;
    ::Display* Display = nullptr;

public:
    X11Context() noexcept;
    ~X11Context() noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize() noexcept;
};

} // namespace Pomdog::Detail::X11
