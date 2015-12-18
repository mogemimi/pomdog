// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/detail/EffectBinaryParameter.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Math/Matrix2x2.hpp"
#include "Pomdog/Math/Matrix3x3.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Quaternion.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cfloat>

namespace Pomdog {
namespace Detail {
namespace EffectBinaryParameter {
namespace {

template <typename T> static
void const* BinaryCast(T const* data)
{
    static_assert(!std::is_pointer<T>::value, "T is not pointer.");
    return static_cast<void const*>(data);
}

template <typename T> static
void* BinaryCast(T * data)
{
    static_assert(!std::is_pointer<T>::value, "T is not pointer.");
    return static_cast<void*>(data);
}

} // unnamed namespace
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Fundamental types(Scalars)
#endif
//-----------------------------------------------------------------------
void Get(ConstantBuffer & constantBuffer, bool & value)
{
    std::uint32_t unsignedValue = 0;
    constantBuffer.GetValue(sizeof(unsignedValue), BinaryCast(&unsignedValue));
    value = (unsignedValue != 0);
}
//-----------------------------------------------------------------------
void Get(ConstantBuffer & constantBuffer, std::int32_t & value)
{
    constantBuffer.GetValue(sizeof(value), BinaryCast(&value));
}
//-----------------------------------------------------------------------
void Get(ConstantBuffer & constantBuffer, std::uint32_t & value)
{
    constantBuffer.GetValue(sizeof(value), BinaryCast(&value));
}
//-----------------------------------------------------------------------
void Get(ConstantBuffer & constantBuffer, float & value)
{
    constantBuffer.GetValue(sizeof(value), BinaryCast(&value));
}
//-----------------------------------------------------------------------
void Get(ConstantBuffer & constantBuffer, double & value)
{
    constantBuffer.GetValue(sizeof(value), BinaryCast(&value));
}
//-----------------------------------------------------------------------
void Set(ConstantBuffer & constantBuffer, bool value)
{
    std::uint32_t const unsignedValue = value ? 1: 0;
    constantBuffer.SetValue(BinaryCast(&unsignedValue), sizeof(unsignedValue));
}
//-----------------------------------------------------------------------
void Set(ConstantBuffer & constantBuffer, std::int32_t value)
{
    constantBuffer.SetValue(BinaryCast(&value), sizeof(value));
}
//-----------------------------------------------------------------------
void Set(ConstantBuffer & constantBuffer, std::uint32_t value)
{
    constantBuffer.SetValue(BinaryCast(&value), sizeof(value));
}
//-----------------------------------------------------------------------
void Set(ConstantBuffer & constantBuffer, float value)
{
    constantBuffer.SetValue(BinaryCast(&value), sizeof(value));
}
//-----------------------------------------------------------------------
void Set(ConstantBuffer & constantBuffer, double value)
{
    constantBuffer.SetValue(BinaryCast(&value), sizeof(value));
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Arrays
#endif
//-----------------------------------------------------------------------
void Set(ConstantBuffer & constantBuffer, std::int32_t const* data, std::uint32_t count)
{
    using type = std::remove_pointer<decltype(data)>::type;
    static_assert(std::is_same<type, std::int32_t const>::value, "");

    POMDOG_ASSERT(count > 0);
    constantBuffer.SetValue(BinaryCast(data), sizeof(type) * count);
}
//-----------------------------------------------------------------------
void Set(ConstantBuffer & constantBuffer, std::uint32_t const* data, std::uint32_t count)
{
    using type = std::remove_pointer<decltype(data)>::type;
    static_assert(std::is_same<type, std::uint32_t const>::value, "");

    POMDOG_ASSERT(count > 0);
    constantBuffer.SetValue(BinaryCast(data), sizeof(type) * count);
}
//-----------------------------------------------------------------------
void Set(ConstantBuffer & constantBuffer, float const* data, std::uint32_t count)
{
    using type = std::remove_pointer<decltype(data)>::type;
    static_assert(std::is_same<type, float const>::value, "");

    POMDOG_ASSERT(count > 0);
    constantBuffer.SetValue(BinaryCast(data), sizeof(type) * count);
}
//-----------------------------------------------------------------------
void Set(ConstantBuffer & constantBuffer, double const* data, std::uint32_t count)
{
    using type = std::remove_pointer<decltype(data)>::type;
    static_assert(std::is_same<type, double const>::value, "");

    POMDOG_ASSERT(count > 0);
    constantBuffer.SetValue(BinaryCast(data), sizeof(type) * count);
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Vectors and Matrices
#endif
//-----------------------------------------------------------------------
template <typename T> void
Set(ConstantBuffer & constantBuffer, FloatingPointVector2<T> const& value)
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number.");
    constantBuffer.SetValue(BinaryCast(value.Data()), sizeof(value));
}
//-----------------------------------------------------------------------
template <typename T> void
Set(ConstantBuffer & constantBuffer, FloatingPointVector3<T> const& value)
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number.");
    constantBuffer.SetValue(BinaryCast(value.Data()), sizeof(value));
}
//-----------------------------------------------------------------------
template <typename T> void
Set(ConstantBuffer & constantBuffer, FloatingPointVector4<T> const& value)
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number.");
    constantBuffer.SetValue(BinaryCast(value.Data()), sizeof(value));
}
//-----------------------------------------------------------------------
template <typename T> void
Set(ConstantBuffer & constantBuffer, FloatingPointMatrix2x2<T> const& value)
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number.");
    constantBuffer.SetValue(BinaryCast(value.Data()), sizeof(value));
}
//-----------------------------------------------------------------------
template <typename T> void
Set(ConstantBuffer & constantBuffer, FloatingPointMatrix3x3<T> const& value)
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number.");
    constantBuffer.SetValue(BinaryCast(value.Data()), sizeof(value));
}
//-----------------------------------------------------------------------
template <typename T> void
Set(ConstantBuffer & constantBuffer, FloatingPointMatrix4x4<T> const& value)
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number.");
    constantBuffer.SetValue(BinaryCast(value.Data()), sizeof(value));
}
//-----------------------------------------------------------------------
template <typename T> void
Set(ConstantBuffer & constantBuffer, FloatingPointQuaternion<T> const& value)
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number.");
    constantBuffer.SetValue(BinaryCast(value.Data()), sizeof(value));
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Explicit instantiations
#endif
//-----------------------------------------------------------------------
template void Set<float>(ConstantBuffer &, FloatingPointVector2<float> const&);
template void Set<float>(ConstantBuffer &, FloatingPointVector3<float> const&);
template void Set<float>(ConstantBuffer &, FloatingPointVector4<float> const&);
template void Set<float>(ConstantBuffer &, FloatingPointMatrix2x2<float> const&);
template void Set<float>(ConstantBuffer &, FloatingPointMatrix3x3<float> const&);
template void Set<float>(ConstantBuffer &, FloatingPointMatrix4x4<float> const&);
template void Set<float>(ConstantBuffer &, FloatingPointQuaternion<float> const&);

#if defined(DBL_MANT_DIG)
template void Set<double>(ConstantBuffer &, FloatingPointVector2<double> const&);
template void Set<double>(ConstantBuffer &, FloatingPointVector3<double> const&);
template void Set<double>(ConstantBuffer &, FloatingPointVector4<double> const&);
template void Set<double>(ConstantBuffer &, FloatingPointMatrix2x2<double> const&);
template void Set<double>(ConstantBuffer &, FloatingPointMatrix3x3<double> const&);
template void Set<double>(ConstantBuffer &, FloatingPointMatrix4x4<double> const&);
template void Set<double>(ConstantBuffer &, FloatingPointQuaternion<double> const&);
#endif
//-----------------------------------------------------------------------
} // namespace EffectBinaryParameter
} // namespace Detail
} // namespace Pomdog
