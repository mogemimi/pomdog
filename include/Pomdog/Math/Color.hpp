//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_COLOR_HPP
#define POMDOG_COLOR_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/FloatingPointColor.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

/////@~English
///// @brief RGBA color.
/////@~Japanese
///// @brief 赤、緑、青、およびアルファ（不透明度）の 4 成分を持つ色を定義します。
using Color = Details::FloatingPointColor<float>;

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_COLOR_HPP)
