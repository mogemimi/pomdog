// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/detail/tagged_arithmetic.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::math::detail {

template <class Tp>
struct IsTaggedFloatingPoint final
    : public std::integral_constant<bool, false> {
};

template <class T, class Tag>
struct IsTaggedFloatingPoint<pomdog::detail::TaggedArithmetic<T, Tag>> final
    : public std::integral_constant<bool, std::is_floating_point<T>::value> {
};

} // namespace pomdog::math::detail
