//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/detail/EffectBinaryParameter.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Math/Vector2.hpp>
#include <Pomdog/Math/Vector3.hpp>
#include <Pomdog/Math/Vector4.hpp>
#include <Pomdog/Math/Matrix2x2.hpp>
#include <Pomdog/Math/Matrix3x3.hpp>
#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Math/Quaternion.hpp>
#include <Pomdog/Math/Color.hpp>
#include <Pomdog/Graphics/EffectParameter.hpp>
#include <cfloat>

namespace Pomdog {
namespace Details {
namespace EffectBinaryParameter {
//-----------------------------------------------------------------------
namespace {

template <typename T> static
std::uint8_t const* BinaryCast(T const* data)
{
	static_assert(!std::is_pointer<T>::value, "T is not pointer.");
	return reinterpret_cast<std::uint8_t const*>(data);
}

template <typename T> static
std::uint8_t * BinaryCast(T * data)
{
	static_assert(!std::is_pointer<T>::value, "T is not pointer.");
	return reinterpret_cast<std::uint8_t *>(data);
}

}// unnamed namespace
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Fundamental types(Scalars)
#endif
//-----------------------------------------------------------------------
void Get(EffectParameter & effectParameter, bool & value)
{
	std::uint32_t unsignedValue = 0;
	effectParameter.GetValue(sizeof(unsignedValue), BinaryCast(&unsignedValue));
	value = (unsignedValue != 0);
}
//-----------------------------------------------------------------------
void Get(EffectParameter & effectParameter, std::int32_t & value)
{
	effectParameter.GetValue(sizeof(value), BinaryCast(&value));
}
//-----------------------------------------------------------------------
void Get(EffectParameter & effectParameter, std::uint32_t & value)
{
	effectParameter.GetValue(sizeof(value), BinaryCast(&value));
}
//-----------------------------------------------------------------------
void Get(EffectParameter & effectParameter, float & value)
{
	effectParameter.GetValue(sizeof(value), BinaryCast(&value));
}
//-----------------------------------------------------------------------
void Get(EffectParameter & effectParameter, double & value)
{
	effectParameter.GetValue(sizeof(value), BinaryCast(&value));
}
//-----------------------------------------------------------------------
void Set(EffectParameter & effectParameter, bool value)
{
	std::uint32_t const unsignedValue = value ? 1: 0;
	effectParameter.SetValue(BinaryCast(&unsignedValue), sizeof(unsignedValue));
}
//-----------------------------------------------------------------------
void Set(EffectParameter & effectParameter, std::int32_t value)
{
	effectParameter.SetValue(BinaryCast(&value), sizeof(value));
}
//-----------------------------------------------------------------------
void Set(EffectParameter & effectParameter, std::uint32_t value)
{
	effectParameter.SetValue(BinaryCast(&value), sizeof(value));
}
//-----------------------------------------------------------------------
void Set(EffectParameter & effectParameter, float value)
{
	effectParameter.SetValue(BinaryCast(&value), sizeof(value));
}
//-----------------------------------------------------------------------
void Set(EffectParameter & effectParameter, double value)
{
	effectParameter.SetValue(BinaryCast(&value), sizeof(value));
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Arrays
#endif
//-----------------------------------------------------------------------
void Set(EffectParameter & effectParameter, std::int32_t const* data, std::uint32_t count)
{
	using type = typename std::remove_pointer<decltype(data)>::type;
	static_assert(std::is_same<type, std::int32_t const>::value, "");
	
	POMDOG_ASSERT(count > 0);
	effectParameter.SetValue(BinaryCast(data), sizeof(type) * count);
}
//-----------------------------------------------------------------------
void Set(EffectParameter & effectParameter, std::uint32_t const* data, std::uint32_t count)
{
	using type = typename std::remove_pointer<decltype(data)>::type;
	static_assert(std::is_same<type, std::uint32_t const>::value, "");
	
	POMDOG_ASSERT(count > 0);
	effectParameter.SetValue(BinaryCast(data), sizeof(type) * count);
}
//-----------------------------------------------------------------------
void Set(EffectParameter & effectParameter, float const* data, std::uint32_t count)
{
	using type = typename std::remove_pointer<decltype(data)>::type;
	static_assert(std::is_same<type, float const>::value, "");
	
	POMDOG_ASSERT(count > 0);
	effectParameter.SetValue(BinaryCast(data), sizeof(type) * count);
}
//-----------------------------------------------------------------------
void Set(EffectParameter & effectParameter, double const* data, std::uint32_t count)
{
	using type = typename std::remove_pointer<decltype(data)>::type;
	static_assert(std::is_same<type, double const>::value, "");
	
	POMDOG_ASSERT(count > 0);
	effectParameter.SetValue(BinaryCast(data), sizeof(type) * count);
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Vectors and Matrices
#endif
//-----------------------------------------------------------------------
template <typename T> void
Set(EffectParameter & effectParameter, FloatingPointVector2<T> const& value)
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number.");
	effectParameter.SetValue(BinaryCast(value.Data()), sizeof(value));
}
//-----------------------------------------------------------------------
template <typename T> void
Set(EffectParameter & effectParameter, FloatingPointVector3<T> const& value)
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number.");
	effectParameter.SetValue(BinaryCast(value.Data()), sizeof(value));
}
//-----------------------------------------------------------------------
template <typename T> void
Set(EffectParameter & effectParameter, FloatingPointVector4<T> const& value)
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number.");
	effectParameter.SetValue(BinaryCast(value.Data()), sizeof(value));
}
//-----------------------------------------------------------------------
template <typename T> void
Set(EffectParameter & effectParameter, FloatingPointMatrix2x2<T> const& value)
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number.");
	effectParameter.SetValue(BinaryCast(value.Data()), sizeof(value));
}
//-----------------------------------------------------------------------
template <typename T> void
Set(EffectParameter & effectParameter, FloatingPointMatrix3x3<T> const& value)
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number.");
	effectParameter.SetValue(BinaryCast(value.Data()), sizeof(value));
}
//-----------------------------------------------------------------------
template <typename T> void
Set(EffectParameter & effectParameter, FloatingPointMatrix4x4<T> const& value)
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number.");
	effectParameter.SetValue(BinaryCast(value.Data()), sizeof(value));
}
//-----------------------------------------------------------------------
template <typename T> void
Set(EffectParameter & effectParameter, FloatingPointQuaternion<T> const& value)
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number.");
	effectParameter.SetValue(BinaryCast(value.Data()), sizeof(value));
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Explicit instantiations
#endif
//-----------------------------------------------------------------------
template void Set<float>(EffectParameter & effectParameter, FloatingPointVector2<float> const& value);
template void Set<float>(EffectParameter & effectParameter, FloatingPointVector3<float> const& value);
template void Set<float>(EffectParameter & effectParameter, FloatingPointVector4<float> const& value);
template void Set<float>(EffectParameter & effectParameter, FloatingPointMatrix2x2<float> const& value);
template void Set<float>(EffectParameter & effectParameter, FloatingPointMatrix3x3<float> const& value);
template void Set<float>(EffectParameter & effectParameter, FloatingPointMatrix4x4<float> const& value);
template void Set<float>(EffectParameter & effectParameter, FloatingPointQuaternion<float> const& value);

#if defined(DBL_MANT_DIG)
template void Set<double>(EffectParameter & effectParameter, FloatingPointVector2<double> const& value);
template void Set<double>(EffectParameter & effectParameter, FloatingPointVector3<double> const& value);
template void Set<double>(EffectParameter & effectParameter, FloatingPointVector4<double> const& value);
template void Set<double>(EffectParameter & effectParameter, FloatingPointMatrix2x2<double> const& value);
template void Set<double>(EffectParameter & effectParameter, FloatingPointMatrix3x3<double> const& value);
template void Set<double>(EffectParameter & effectParameter, FloatingPointMatrix4x4<double> const& value);
template void Set<double>(EffectParameter & effectParameter, FloatingPointQuaternion<double> const& value);
#endif

#if defined(LDBL_MANT_DIG)
// Notes: Long-double value is not supported.
//template void Set<long double>(EffectParameter & effectParameter, FloatingPointVector2<long double> const& value);
//template void Set<long double>(EffectParameter & effectParameter, FloatingPointVector3<long double> const& value);
//template void Set<long double>(EffectParameter & effectParameter, FloatingPointVector4<long double> const& value);
//template void Set<long double>(EffectParameter & effectParameter, FloatingPointMatrix2x2<long double> const& value);
//template void Set<long double>(EffectParameter & effectParameter, FloatingPointMatrix3x3<long double> const& value);
//template void Set<long double>(EffectParameter & effectParameter, FloatingPointMatrix4x4<long double> const& value);
//template void Set<long double>(EffectParameter & effectParameter, FloatingPointQuaternion<long double> const& value);
#endif
//-----------------------------------------------------------------------
}// namespace EffectBinaryParameter
}// namespace Details
}// namespace Pomdog
