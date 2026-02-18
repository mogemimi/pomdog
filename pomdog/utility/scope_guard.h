// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <concepts>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Represents a scope guard that executes a given function when it goes out of scope.
template <typename Callable>
    requires std::invocable<Callable> && requires(Callable f) {{f()} -> std::same_as<void>; }
class ScopeGuard final {
private:
    Callable func_;
    bool active_ = true;

public:
    ScopeGuard() noexcept = delete;
    ScopeGuard(const ScopeGuard&) noexcept = delete;
    ScopeGuard& operator=(const ScopeGuard&) noexcept = delete;
    ScopeGuard& operator=(ScopeGuard&&) noexcept = delete;

    /// Constructs a ScopeGuard with the given function `f`.
    /// The function will be executed when the ScopeGuard goes out of scope, unless it is dismissed.
    template <typename Function>
    explicit ScopeGuard(Function&& f) noexcept
        : func_(std::forward<Function>(f))
    {
    }

    /// Move constructor that transfers ownership of the function from `other` to the new ScopeGuard.
    ScopeGuard(ScopeGuard&& other) noexcept
        : func_(std::move(other.func_))
        , active_(other.active_)
    {
        other.dismiss();
    }

    /// Destructs the ScopeGuard and executes the function if it has not been dismissed.
    ~ScopeGuard() noexcept
    {
        if (active_) {
            func_();
        }
    }

    /// Dismisses the ScopeGuard, preventing the function from being executed when the ScopeGuard goes out of scope.
    void dismiss() noexcept
    {
        active_ = false;
    }
};

/// Deduction guide for ScopeGuard to allow template argument deduction from the constructor argument.
template <typename Callable>
ScopeGuard(Callable) -> ScopeGuard<Callable>;

} // namespace pomdog
