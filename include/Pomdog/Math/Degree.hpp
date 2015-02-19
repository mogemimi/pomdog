//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_DEGREE_8B079286_FF3F_4EEC_BDD2_85D914A0B064_HPP
#define POMDOG_DEGREE_8B079286_FF3F_4EEC_BDD2_85D914A0B064_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/TaggedArithmetic.hpp"

namespace Pomdog {
namespace Details {
namespace Tags {

struct DegreeTag {};

}// namespace Tags
}// namespace Details

template <typename T>
using Degree = Details::TaggedArithmetic<T, Details::Tags::DegreeTag>;

}// namespace Pomdog

#endif // !defined(POMDOG_DEGREE_8B079286_FF3F_4EEC_BDD2_85D914A0B064_HPP)
