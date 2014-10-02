//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_JOINTINDEX_F35DB555_D353_45D1_938C_AF77D680DD52_HPP
#define POMDOG_JOINTINDEX_F35DB555_D353_45D1_938C_AF77D680DD52_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog/Utility/Assert.hpp"
#include <cstdint>
#include <type_traits>
#include <limits>

namespace Pomdog {
namespace Details {
namespace SkeletalAnimation2D {

template <typename T>
class OptionalUnsigned final {
public:
	static_assert(std::is_unsigned<T>::value, "T is unsigned integer type.");
	T Data;
	
	// Constructors:
	constexpr OptionalUnsigned()
		: Data(std::numeric_limits<T>::max())
	{}
	
	constexpr OptionalUnsigned(T v)
		: Data(v)
	{}
	
	// Binary operators:
	constexpr bool operator==(OptionalUnsigned const& v) const
	{
		return POMDOG_CONSTEXPR_ASSERT(Data != std::numeric_limits<T>::max()),
			Data == v.Data;
	}

	constexpr bool operator!=(OptionalUnsigned const& v) const
	{
		return POMDOG_CONSTEXPR_ASSERT(Data != std::numeric_limits<T>::max()),
			Data != v.Data;
	}
		
	T const& operator*() const
	{
		POMDOG_ASSERT(Data != std::numeric_limits<T>::max());
		return Data;
	}
	
	T & operator*()
	{
		POMDOG_ASSERT(Data != std::numeric_limits<T>::max());
		return Data;
	}
	
	constexpr explicit operator bool() const noexcept
	{
		return Data != std::numeric_limits<T>::max();
	}
};

}// namespace SkeletalAnimation2D
}// namespace Details

///@~Japanese
/// @brief スケルタルアニメーションにおけるジョイント（間接）に割り当てられるインデックス番号です。
using JointIndex = Details::SkeletalAnimation2D::OptionalUnsigned<std::uint8_t>;

}// namespace Pomdog

#endif // !defined(POMDOG_JOINTINDEX_F35DB555_D353_45D1_938C_AF77D680DD52_HPP)
