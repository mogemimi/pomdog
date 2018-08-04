// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/detail/EffectBinaryParameter.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Matrix2x2.hpp"
#include "Pomdog/Math/Matrix3x3.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Quaternion.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cfloat>

namespace Pomdog {
namespace Detail {
namespace EffectBinaryParameter {
namespace {

template <typename T> static
const void* BinaryCast(const T* data)
{
    static_assert(!std::is_pointer<T>::value, "T is not pointer.");
    return static_cast<const void*>(data);
}

template <typename T> static
void* BinaryCast(T * data)
{
    static_assert(!std::is_pointer<T>::value, "T is not pointer.");
    return static_cast<void*>(data);
}

} // unnamed namespace

// MARK: - Fundamental types(Scalars)

void Get(ConstantBuffer & constantBuffer, bool & value)
{
    std::uint32_t unsignedValue = 0;
    constantBuffer.GetValue(sizeof(unsignedValue), BinaryCast(&unsignedValue));
    value = (unsignedValue != 0);
}

void Get(ConstantBuffer & constantBuffer, std::int32_t & value)
{
    constantBuffer.GetValue(sizeof(value), BinaryCast(&value));
}

void Get(ConstantBuffer & constantBuffer, std::uint32_t & value)
{
    constantBuffer.GetValue(sizeof(value), BinaryCast(&value));
}

void Get(ConstantBuffer & constantBuffer, float & value)
{
    constantBuffer.GetValue(sizeof(value), BinaryCast(&value));
}

void Get(ConstantBuffer & constantBuffer, double & value)
{
    constantBuffer.GetValue(sizeof(value), BinaryCast(&value));
}

void Set(ConstantBuffer & constantBuffer, bool value)
{
    std::uint32_t const unsignedValue = value ? 1: 0;
    constantBuffer.SetValue(BinaryCast(&unsignedValue), sizeof(unsignedValue));
}

void Set(ConstantBuffer & constantBuffer, std::int32_t value)
{
    constantBuffer.SetValue(BinaryCast(&value), sizeof(value));
}

void Set(ConstantBuffer & constantBuffer, std::uint32_t value)
{
    constantBuffer.SetValue(BinaryCast(&value), sizeof(value));
}

void Set(ConstantBuffer & constantBuffer, float value)
{
    constantBuffer.SetValue(BinaryCast(&value), sizeof(value));
}

void Set(ConstantBuffer & constantBuffer, double value)
{
    constantBuffer.SetValue(BinaryCast(&value), sizeof(value));
}

// MARK: - Arrays

void Set(ConstantBuffer & constantBuffer, const std::int32_t* data, std::uint32_t count)
{
    using type = std::remove_pointer<decltype(data)>::type;
    static_assert(std::is_same<type, std::int32_t const>::value, "");

    POMDOG_ASSERT(count > 0);
    constantBuffer.SetValue(BinaryCast(data), sizeof(type) * count);
}

void Set(ConstantBuffer & constantBuffer, const std::uint32_t* data, std::uint32_t count)
{
    using type = std::remove_pointer<decltype(data)>::type;
    static_assert(std::is_same<type, std::uint32_t const>::value, "");

    POMDOG_ASSERT(count > 0);
    constantBuffer.SetValue(BinaryCast(data), sizeof(type) * count);
}

void Set(ConstantBuffer & constantBuffer, const float* data, std::uint32_t count)
{
    using type = std::remove_pointer<decltype(data)>::type;
    static_assert(std::is_same<type, float const>::value, "");

    POMDOG_ASSERT(count > 0);
    constantBuffer.SetValue(BinaryCast(data), sizeof(type) * count);
}

void Set(ConstantBuffer & constantBuffer, const double* data, std::uint32_t count)
{
    using type = std::remove_pointer<decltype(data)>::type;
    static_assert(std::is_same<type, double const>::value, "");

    POMDOG_ASSERT(count > 0);
    constantBuffer.SetValue(BinaryCast(data), sizeof(type) * count);
}

// MARK: - Vectors and Matrices

template <typename T> void
Set(ConstantBuffer & constantBuffer, const FloatingPointVector2<T>& value)
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number.");
    constantBuffer.SetValue(BinaryCast(value.Data()), sizeof(value));
}

template <typename T> void
Set(ConstantBuffer & constantBuffer, const FloatingPointVector3<T>& value)
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number.");
    constantBuffer.SetValue(BinaryCast(value.Data()), sizeof(value));
}

template <typename T> void
Set(ConstantBuffer & constantBuffer, const FloatingPointVector4<T>& value)
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number.");
    constantBuffer.SetValue(BinaryCast(value.Data()), sizeof(value));
}

template <typename T> void
Set(ConstantBuffer & constantBuffer, const FloatingPointMatrix2x2<T>& value)
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number.");
    constantBuffer.SetValue(BinaryCast(value.Data()), sizeof(value));
}

template <typename T> void
Set(ConstantBuffer & constantBuffer, const FloatingPointMatrix3x3<T>& value)
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number.");
    constantBuffer.SetValue(BinaryCast(value.Data()), sizeof(value));
}

template <typename T> void
Set(ConstantBuffer & constantBuffer, const FloatingPointMatrix4x4<T>& value)
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number.");
    constantBuffer.SetValue(BinaryCast(value.Data()), sizeof(value));
}

template <typename T> void
Set(ConstantBuffer & constantBuffer, const FloatingPointQuaternion<T>& value)
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number.");
    constantBuffer.SetValue(BinaryCast(value.Data()), sizeof(value));
}

// MARK: - Explicit instantiations

template void Set<float>(ConstantBuffer &, const FloatingPointVector2<float>&);
template void Set<float>(ConstantBuffer &, const FloatingPointVector3<float>&);
template void Set<float>(ConstantBuffer &, const FloatingPointVector4<float>&);
template void Set<float>(ConstantBuffer &, const FloatingPointMatrix2x2<float>&);
template void Set<float>(ConstantBuffer &, const FloatingPointMatrix3x3<float>&);
template void Set<float>(ConstantBuffer &, const FloatingPointMatrix4x4<float>&);
template void Set<float>(ConstantBuffer &, const FloatingPointQuaternion<float>&);

#if defined(DBL_MANT_DIG)
template void Set<double>(ConstantBuffer &, const FloatingPointVector2<double>&);
template void Set<double>(ConstantBuffer &, const FloatingPointVector3<double>&);
template void Set<double>(ConstantBuffer &, const FloatingPointVector4<double>&);
template void Set<double>(ConstantBuffer &, const FloatingPointMatrix2x2<double>&);
template void Set<double>(ConstantBuffer &, const FloatingPointMatrix3x3<double>&);
template void Set<double>(ConstantBuffer &, const FloatingPointMatrix4x4<double>&);
template void Set<double>(ConstantBuffer &, const FloatingPointQuaternion<double>&);
#endif

} // namespace EffectBinaryParameter
} // namespace Detail
} // namespace Pomdog
