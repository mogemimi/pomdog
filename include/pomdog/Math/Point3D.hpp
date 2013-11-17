//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_POINT3D_HPP
#define POMDOG_POINT3D_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "detail/Coordinate3D.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

typedef Details::Coordinate3D<std::int32_t> Point3D;

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_POINT3D_HPP)
