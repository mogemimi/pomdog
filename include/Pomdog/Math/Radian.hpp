// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RADIAN_671670CD_HPP
#define POMDOG_RADIAN_671670CD_HPP

#include "detail/TaggedArithmetic.hpp"

namespace Pomdog {
namespace Detail {
namespace Tags {

struct RadianTag {};

}// namespace Tags
}// namespace Detail

template <typename T>
using Radian = Detail::TaggedArithmetic<T, Detail::Tags::RadianTag>;

}// namespace Pomdog

#endif // POMDOG_RADIAN_671670CD_HPP
