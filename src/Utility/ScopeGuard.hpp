// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Utility/Assert.hpp"
#include <functional>

namespace Pomdog::Detail {

class ScopeGuard final {
public:
    ScopeGuard() = delete;
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

    ScopeGuard(ScopeGuard&&) = default;
    ScopeGuard& operator=(ScopeGuard&&) = default;

    explicit ScopeGuard(std::function<void()>&& f) noexcept;

    ~ScopeGuard() noexcept;

    void Dismiss() noexcept;

private:
    std::function<void()> func;
};

} // namespace Pomdog::Detail
