//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_GRAPHICS_EFFECTBINARYPARAMETER_F5EA7DB2_2233_4221_973E_189E6ECFB1F0_HPP
#define POMDOG_DETAIL_GRAPHICS_EFFECTBINARYPARAMETER_F5EA7DB2_2233_4221_973E_189E6ECFB1F0_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Config/Export.hpp"
#include <type_traits>

namespace Pomdog {

class EffectParameter;

namespace Details {
namespace EffectBinaryParameter {

void POMDOG_EXPORT Get(EffectParameter const& effectParameter, bool & result);
void POMDOG_EXPORT Get(EffectParameter const& effectParameter, std::int32_t & result);
void POMDOG_EXPORT Get(EffectParameter const& effectParameter, std::uint32_t & result);
void POMDOG_EXPORT Get(EffectParameter const& effectParameter, float & result);
void POMDOG_EXPORT Get(EffectParameter const& effectParameter, double & result);

template <class T, class EffectParameter>
void Get(EffectParameter const& effectParameter, T & result)
{
	static_assert(std::is_pod<T>::value, "You can only use POD value.");
	effectParameter.GetValue(sizeof(result), static_cast<void*>(&result));
}

void POMDOG_EXPORT Set(EffectParameter & effectParameter, bool value);
void POMDOG_EXPORT Set(EffectParameter & effectParameter, std::int32_t value);
void POMDOG_EXPORT Set(EffectParameter & effectParameter, std::uint32_t value);
void POMDOG_EXPORT Set(EffectParameter & effectParameter, float value);
void POMDOG_EXPORT Set(EffectParameter & effectParameter, double value);

//void POMDOG_EXPORT Set(EffectParameter & effectParameter, bool const* data, std::uint32_t count);
void POMDOG_EXPORT Set(EffectParameter & effectParameter, std::int32_t const* data, std::uint32_t count);
void POMDOG_EXPORT Set(EffectParameter & effectParameter, std::uint32_t const* data, std::uint32_t count);
void POMDOG_EXPORT Set(EffectParameter & effectParameter, float const* data, std::uint32_t count);
void POMDOG_EXPORT Set(EffectParameter & effectParameter, double const* data, std::uint32_t count);

template <typename T> void POMDOG_EXPORT Set(EffectParameter & effectParameter, FloatingPointVector2<T> const& value);
template <typename T> void POMDOG_EXPORT Set(EffectParameter & effectParameter, FloatingPointVector3<T> const& value);
template <typename T> void POMDOG_EXPORT Set(EffectParameter & effectParameter, FloatingPointVector4<T> const& value);
template <typename T> void POMDOG_EXPORT Set(EffectParameter & effectParameter, FloatingPointMatrix2x2<T> const& value);
template <typename T> void POMDOG_EXPORT Set(EffectParameter & effectParameter, FloatingPointMatrix3x3<T> const& value);
template <typename T> void POMDOG_EXPORT Set(EffectParameter & effectParameter, FloatingPointMatrix4x4<T> const& value);
template <typename T> void POMDOG_EXPORT Set(EffectParameter & effectParameter, FloatingPointQuaternion<T> const& value);

template <class T, class EffectParameter>
void Set(EffectParameter & effectParameter, T const& value)
{
	static_assert(std::is_pod<T>::value, "You can only use POD value.");
	effectParameter.SetValue(static_cast<void const*>(&value), sizeof(value));
}

}// namespace EffectBinaryParameter
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_GRAPHICS_EFFECTBINARYPARAMETER_F5EA7DB2_2233_4221_973E_189E6ECFB1F0_HPP)
