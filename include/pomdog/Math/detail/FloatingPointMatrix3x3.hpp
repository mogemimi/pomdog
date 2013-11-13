//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_FLOATINGPOINTMATRIX3X3_HPP
#define POMDOG_DETAIL_FLOATINGPOINTMATRIX3X3_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../../Config/FundamentalTypes.hpp"
#include <type_traits>
#include <array>

namespace Pomdog {
namespace Details {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~Japanese
/// @brief 4x4 の浮動小数点数の行列を定義します。
template <typename T>
class FloatingPointMatrix3x3
{
public:
	static_assert(std::is_floating_point<T>::value, "T is floating point.");
	typedef T value_type;
	
	std::array<std::array<T, 3>, 3> m;

public:
	// Constructors:
	FloatingPointMatrix3x3() = default;
	FloatingPointMatrix3x3(FloatingPointMatrix3x3 const&) = default;
	FloatingPointMatrix3x3(FloatingPointMatrix3x3 &&) = default;
	
	virtual ~FloatingPointMatrix3x3() = default;
	
	// Assignment operators:
	FloatingPointMatrix3x3 & operator=(FloatingPointMatrix3x3 const&) = default;
	FloatingPointMatrix3x3 & operator=(FloatingPointMatrix3x3 &&) = default;
};

/// @}
/// @}

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_FLOATINGPOINTMATRIX3X3_HPP)
