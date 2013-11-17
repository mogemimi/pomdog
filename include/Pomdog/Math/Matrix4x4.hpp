//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MATRIX4X4_HPP
#define POMDOG_MATRIX4X4_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/FloatingPointMatrix4x4.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~Japanese
/// @brief 4x4 の float 型の行列を定義します。
typedef Details::FloatingPointMatrix4x4<float> Matrix4x4;

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_MATRIX4X4_HPP)
