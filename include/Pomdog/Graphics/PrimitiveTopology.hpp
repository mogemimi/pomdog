//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PRIMITIVETOPOLOGY_46F3C71C_626F_43D4_8CBD_F4FC28EE1933_HPP
#define POMDOG_PRIMITIVETOPOLOGY_46F3C71C_626F_43D4_8CBD_F4FC28EE1933_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief プリミティブの位相を定義します。
enum class PrimitiveTopology: std::uint8_t
{
	///@brief A triangle list.
	TriangleList,
	
	///@brief A triangle strip.
	TriangleStrip,
	
	///@brief A line list.
	LineList,
	
	///@brief A line strip.
	LineStrip,
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_PRIMITIVETOPOLOGY_46F3C71C_626F_43D4_8CBD_F4FC28EE1933_HPP)
