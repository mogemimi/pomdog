//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MATRIX2X2_HPP
#define POMDOG_MATRIX2X2_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/FloatingPointMatrix2x2.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~Japanese
/// @brief 2x2 の float 型の行列を定義します。
typedef Details::FloatingPointMatrix2x2<float> Matrix2x2;

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_MATRIX2X2_HPP)
