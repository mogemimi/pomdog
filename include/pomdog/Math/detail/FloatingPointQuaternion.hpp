//
//  Copyright (C) 2013 the Pomdog Engine authors.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_FLOATINGPOINTQUATERNION_HPP
#define POMDOG_DETAIL_FLOATINGPOINTQUATERNION_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../../Config/FundamentalTypes.hpp"
#include <type_traits>

namespace Pomdog {
namespace Details {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~Japanese
/// @brief 四元数（クォータニオン）を表す 4 次元ベクトルを定義します。
template <typename T>
class FloatingPointQuaternion
{
public:
	static_assert(std::is_floating_point<T>::value, "T is floating point.");
	typedef T value_type;

	T x, y, z, w;

public:
	FloatingPointQuaternion() = default;
	FloatingPointQuaternion(FloatingPointQuaternion const&) = default;
	FloatingPointQuaternion(FloatingPointQuaternion &&) = default;
	
	virtual ~FloatingPointQuaternion() = default;
	
	// assignment operators
	FloatingPointQuaternion & operator=(FloatingPointQuaternion const&) = default;
	FloatingPointQuaternion & operator=(FloatingPointQuaternion &&) = default;
};

/// @}
/// @}

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_FLOATINGPOINTQUATERNION_HPP)
