// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/scope_guard.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

ScopeGuard::ScopeGuard(std::function<void()>&& f) noexcept
    : func_(std::move(f))
{
}

ScopeGuard::~ScopeGuard() noexcept
{
    if (func_) {
        func_();
    }
}

void ScopeGuard::dismiss() noexcept
{
    func_ = nullptr;
}

} // namespace pomdog::detail
