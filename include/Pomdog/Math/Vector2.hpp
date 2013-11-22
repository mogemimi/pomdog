//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_VECTOR2_HPP
#define POMDOG_VECTOR2_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/FloatingPointVector2.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~English
/// @brief vector in two-dimensional space.
///@~Japanese
/// @brief 2 つの要素を持つベクトル（2 次元ベクトル）を定義します。
using Vector2 = Details::FloatingPointVector2<float>;

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_VECTOR2_HPP)
