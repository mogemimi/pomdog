// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "ScopeGuard.hpp"
#include <utility>

namespace Pomdog::Detail {

ScopeGuard::ScopeGuard(std::function<void()>&& f) noexcept
    : func(std::move(f))
{
}

ScopeGuard::~ScopeGuard() noexcept
{
    if (func) {
        func();
    }
}

void ScopeGuard::Dismiss() noexcept
{
    func = nullptr;
}

} // namespace Pomdog::Detail
