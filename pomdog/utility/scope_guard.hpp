// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

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

} // namespace pomdog::detail
