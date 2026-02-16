// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

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

} // namespace pomdog::detail
