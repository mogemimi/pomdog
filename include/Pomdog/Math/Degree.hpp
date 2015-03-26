// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_DEGREE_8B079286_HPP
#define POMDOG_DEGREE_8B079286_HPP

#include "detail/TaggedArithmetic.hpp"

namespace Pomdog {
namespace Detail {
namespace Tags {

struct DegreeTag {};

}// namespace Tags
}// namespace Detail

template <typename T>
using Degree = Detail::TaggedArithmetic<T, Detail::Tags::DegreeTag>;

}// namespace Pomdog

#endif // POMDOG_DEGREE_8B079286_HPP
