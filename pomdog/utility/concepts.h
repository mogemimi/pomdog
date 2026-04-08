// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::concepts {

/// Checks if Derived is derived from Base and is convertible to Base.
template <class Derived, class Base>
concept derived_from =
    std::is_base_of_v<Base, Derived> &&
    std::is_convertible_v<const Derived*, const Base*>;

/// Checks if From is convertible to To.
template <class From, class To>
concept convertible_to =
    std::is_convertible_v<From, To> &&
    requires(std::add_rvalue_reference_t<From> (&f)()) {
        static_cast<To>(f());
    };

namespace detail {

/// Primary template: defaults to false for non-matching signatures.
template <typename Func, typename Signature>
struct callable_as_impl : std::false_type {};

/// Specialization: true if Func is invocable with Args... and return type is convertible to R.
template <typename Func, typename R, typename... Args>
struct callable_as_impl<Func, R(Args...)>
    : std::bool_constant<std::is_invocable_r_v<R, Func, Args...>> {};

} // namespace detail

/// Checks whether Func can be called with the given function signature.
template <typename Func, typename Signature>
concept callable_as = detail::callable_as_impl<Func, Signature>::value;

} // namespace pomdog::concepts
