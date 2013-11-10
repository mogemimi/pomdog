//
//  Copyright (C) 2013 the Pomdog Engine authors.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_FLOATINGPOINTMATRIX4X4_HPP
#define POMDOG_DETAIL_FLOATINGPOINTMATRIX4X4_HPP

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
/// @brief 4x4 の float 型の行列を定義します。
template <typename T>
class FloatingPointMatrix4x4
{
public:
	static_assert(std::is_floating_point<T>::value, "T is floating point.");
	typedef T value_type;
	
	std::array<std::array<T, 4>, 4> m;

public:
	FloatingPointMatrix4x4() = default;
	FloatingPointMatrix4x4(FloatingPointMatrix4x4 const&) = default;
	FloatingPointMatrix4x4(FloatingPointMatrix4x4 &&) = default;
	
	virtual ~FloatingPointMatrix4x4() = default;
	
	// assignment operators
	FloatingPointMatrix4x4 & operator=(FloatingPointMatrix4x4 const&) = default;
	FloatingPointMatrix4x4 & operator=(FloatingPointMatrix4x4 &&) = default;
};

/// @}
/// @}

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_FLOATINGPOINTMATRIX4X4_HPP)
