//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MATRIX3X3_HPP
#define POMDOG_MATRIX3X3_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/FloatingPointMatrix3x3.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~Japanese
/// @brief 3x3 の float 型の行列を定義します。
using Matrix3x3 = Details::FloatingPointMatrix3x3<float>;

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_MATRIX3X3_HPP)
