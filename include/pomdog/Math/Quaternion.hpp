//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_QUATERNION_HPP
#define POMDOG_QUATERNION_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/FloatingPointQuaternion.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~Japanese
/// @brief 四元数（クォータニオン）を表す 4 次元ベクトルを定義します。
typedef Details::FloatingPointQuaternion<float> Quaternion;

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_QUATERNION_HPP)
