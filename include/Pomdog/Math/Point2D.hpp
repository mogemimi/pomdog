//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_POINT2D_HPP
#define POMDOG_POINT2D_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "detail/Coordinate2D.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

using Point2D = Details::Coordinate2D<std::int32_t>;

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_POINT2D_HPP)
