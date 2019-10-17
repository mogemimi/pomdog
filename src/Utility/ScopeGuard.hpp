// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Utility/Assert.hpp"
#include <functional>
#include <utility>

namespace Pomdog::Detail {

class ScopeGuard final {
public:
    ScopeGuard() = delete;
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard(ScopeGuard&&) = default;

    explicit ScopeGuard(const std::function<void()>& f)
        : func(f)
    {
    }

    explicit ScopeGuard(std::function<void()>&& f)
        : func(std::move(f))
    {
    }

    ~ScopeGuard()
    {
        POMDOG_ASSERT(func);
        func();
    }

    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ScopeGuard& operator=(ScopeGuard&&) = default;

private:
    std::function<void()> func;
};

} // namespace Pomdog::Detail
