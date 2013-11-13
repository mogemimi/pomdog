//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_RADIAN_HPP
#define POMDOG_RADIAN_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "detail/TaggedArithmetic.hpp"

namespace Pomdog {
namespace Details {
namespace Tags {

struct radian_tag {};

}// namespace Tags
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

template <typename T>
using Radian = Details::TaggedArithmetic<T, Details::Tags::radian_tag, std::is_floating_point<T>::value>;

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_RADIAN_HPP)
