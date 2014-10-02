//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_VECTOR3_7F01F272_CEB3_49AB_8F83_FF493EFD0757_HPP
#define POMDOG_VECTOR3_7F01F272_CEB3_49AB_8F83_FF493EFD0757_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/FloatingPointVector3.hpp"

namespace Pomdog {

///@~English
/// @brief vector in three-dimensional space.
///@~Japanese
/// @brief 3 つの要素を持つベクトル（3 次元ベクトル）を定義します。
using Vector3 = Details::FloatingPointVector3<float>;

}// namespace Pomdog

#endif // !defined(POMDOG_VECTOR3_7F01F272_CEB3_49AB_8F83_FF493EFD0757_HPP)
