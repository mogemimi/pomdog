//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_RADIAN_671670CD_1B1C_4719_B0A1_D666BA07A0A9_HPP
#define POMDOG_RADIAN_671670CD_1B1C_4719_B0A1_D666BA07A0A9_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "detail/TaggedArithmetic.hpp"

namespace Pomdog {
namespace Details {
namespace Tags {

struct RadianTag {};

}// namespace Tags
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

template <typename T>
using Radian = Details::TaggedArithmetic<T, Details::Tags::RadianTag>;

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_RADIAN_671670CD_1B1C_4719_B0A1_D666BA07A0A9_HPP)
