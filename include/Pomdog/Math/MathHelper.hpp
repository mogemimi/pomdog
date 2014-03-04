//
//  Copyright (C) 2013-2014 mogemimi.
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

#include <cstdint>
#include <cfloat>
#include <type_traits>

namespace Pomdog {

template <typename T>
struct MathConstants;

template <>
struct MathConstants<float>
{
	static constexpr float Pi()         { return 3.1415926535f; }
	static constexpr float TwoPi()      { return 6.2831853071f; }
	static constexpr float OneOverPi()  { return 0.3183098861f; }
	static constexpr float OneOver2Pi() { return 0.1591549430f; }
	static constexpr float PiOver2()    { return 1.5707963267f; }
	static constexpr float PiOver4()    { return 0.7853981633f; }
};

namespace MathHelper {
//-------------------------------------------------------------------
template <typename T>
T Clamp(T x, T minval, T maxval)
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
T Saturate(T x)
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
T Lerp(T source0, T source1, T amount)
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
T SmoothStep(T min, T max, T amount)
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
