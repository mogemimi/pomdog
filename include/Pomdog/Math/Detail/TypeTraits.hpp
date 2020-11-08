// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Detail/TaggedArithmetic.hpp"
#include <type_traits>

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
