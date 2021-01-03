// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Math/Detail/TaggedArithmetic.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Math::Detail {

template <class Tp>
struct IsTaggedFloatingPoint final
    : public std::integral_constant<bool, false> {
};

template <class T, class Tag>
struct IsTaggedFloatingPoint<Pomdog::Detail::TaggedArithmetic<T, Tag>> final
    : public std::integral_constant<bool, std::is_floating_point<T>::value> {
};

} // namespace Pomdog::Math::Detail
