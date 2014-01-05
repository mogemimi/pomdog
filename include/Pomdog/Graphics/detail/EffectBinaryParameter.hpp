//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EFFECTBINARYPARAMETER_F5EA7DB2_2233_4221_973E_189E6ECFB1F0_HPP
#define POMDOG_EFFECTBINARYPARAMETER_F5EA7DB2_2233_4221_973E_189E6ECFB1F0_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <type_traits>
#include <Pomdog/Config/Export.hpp>
#include <Pomdog/Math/detail/ForwardDeclarations.hpp>

namespace Pomdog {

class EffectParameter;

namespace Details {
namespace EffectBinaryParameter {

void POMDOG_EXPORT Set(EffectParameter & effectParameter, bool value);
void POMDOG_EXPORT Set(EffectParameter & effectParameter, std::int32_t value);
void POMDOG_EXPORT Set(EffectParameter & effectParameter, std::uint32_t value);
void POMDOG_EXPORT Set(EffectParameter & effectParameter, float value);
void POMDOG_EXPORT Set(EffectParameter & effectParameter, double value);

template <typename T> void POMDOG_EXPORT Set(EffectParameter & effectParameter, FloatingPointVector2<T> const& value);
template <typename T> void POMDOG_EXPORT Set(EffectParameter & effectParameter, FloatingPointVector3<T> const& value);
template <typename T> void POMDOG_EXPORT Set(EffectParameter & effectParameter, FloatingPointVector4<T> const& value);
template <typename T> void POMDOG_EXPORT Set(EffectParameter & effectParameter, FloatingPointMatrix2x2<T> const& value);
template <typename T> void POMDOG_EXPORT Set(EffectParameter & effectParameter, FloatingPointMatrix3x3<T> const& value);
template <typename T> void POMDOG_EXPORT Set(EffectParameter & effectParameter, FloatingPointMatrix4x4<T> const& value);
template <typename T> void POMDOG_EXPORT Set(EffectParameter & effectParameter, FloatingPointColor<T> const& value);
template <typename T> void POMDOG_EXPORT Set(EffectParameter & effectParameter, FloatingPointQuaternion<T> const& value);

template <class EffectParameter, class T>
void Set(EffectParameter & effectParameter, T const& value)
{
	static_assert(std::is_pod<T>::value, "You can only use POD value.");
	effectParameter.SetValue(reinterpret_cast<std::uint8_t const*>(&value), sizeof(value));
}

}// namespace EffectBinaryParameter
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTBINARYPARAMETER_F5EA7DB2_2233_4221_973E_189E6ECFB1F0_HPP)
