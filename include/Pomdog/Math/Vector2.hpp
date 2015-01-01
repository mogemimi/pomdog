//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_VECTOR2_387CFCB7_CB0A_410C_B5B2_506C746C3E2D_HPP
#define POMDOG_VECTOR2_387CFCB7_CB0A_410C_B5B2_506C746C3E2D_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "detail/FloatingPointVector2.hpp"

namespace Pomdog {

///@~English
/// @brief vector in two-dimensional space.
///@~Japanese
/// @brief 2 つの要素を持つベクトル（2 次元ベクトル）を定義します。
using Vector2 = Details::FloatingPointVector2<float>;

}// namespace Pomdog

#endif // !defined(POMDOG_VECTOR2_387CFCB7_CB0A_410C_B5B2_506C746C3E2D_HPP)
