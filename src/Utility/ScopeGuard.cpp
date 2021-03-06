// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "ScopeGuard.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
