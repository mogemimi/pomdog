//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MATHHELPER_F07877B3_827B_4997_8895_51BD6934A0B7_HPP
#define POMDOG_MATHHELPER_F07877B3_827B_4997_8895_51BD6934A0B7_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include <type_traits>

namespace Pomdog {
namespace MathHelper {

//-------------------------------------------------------------------
template <typename T>
static T Clamp(T x, T minval, T maxval)
{
	static_assert(std::is_arithmetic<T>::value, "");
	POMDOG_ASSERT_MESSAGE(minval < maxval, "In Clamp, maxval is out of range");
	if (x < minval)
		return minval;
	else if (x > maxval)
		return maxval;
	return x;
}
//-------------------------------------------------------------------
template <typename T>
static T Saturate(T x)
{
	static_assert(std::is_floating_point<T>::value, "T is floaing point number");
	constexpr T Zero = 0;
	constexpr T One = 1;
	return Clamp(x, Zero, One);
}
//-------------------------------------------------------------------
///@~Japanese
/// @brief 線形補間を行います。
template <typename T>
static T Lerp(T source0, T source1, T amount)
{
	static_assert(std::is_floating_point<T>::value, "T is floaing point number");
	//return ((1 - amount) * source0) + (amount * source1);
	//return source0 + (amount * (-source0)) + (amount * source1);
	return source0 + amount * (source1 - source0);
}
//-------------------------------------------------------------------
///@~Japanese
/// @brief スムーズなエルミート補間を行います
/// @remarks amount が [min, max] の範囲内であれば、[0, 1] の間のスムーズなエルミート補間を行います。
template <typename T>
static T SmoothStep(T min, T max, T amount)
{
	static_assert(std::is_floating_point<T>::value, "T is floaing point number");
	// Scale, bias and saturate amount to 0...1 range
	auto const x = Saturate((amount - min)/(max- min));
	// Evaluate polynomial
	return x*x*(3-2*x);
}

}// namespace MathHelper
}// namespace Pomdog

#endif // !defined(POMDOG_MATHHELPER_F07877B3_827B_4997_8895_51BD6934A0B7_HPP)
