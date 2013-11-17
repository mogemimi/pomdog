//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_VECTOR3_HPP
#define POMDOG_VECTOR3_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/FloatingPointVector3.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~English
/// @brief vector in three-dimensional space.
///@~Japanese
/// @brief 3 つの要素を持つベクトル（3 次元ベクトル）を定義します。
typedef Details::FloatingPointVector3<float> Vector3;

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_VECTOR3_HPP)
