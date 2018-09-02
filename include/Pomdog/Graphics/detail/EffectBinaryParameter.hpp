// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <type_traits>

namespace Pomdog {

class ConstantBuffer;

namespace Detail {
namespace EffectBinaryParameter {

void POMDOG_EXPORT Get(const ConstantBuffer& constantBuffer, bool& result);
void POMDOG_EXPORT Get(const ConstantBuffer& constantBuffer, std::int32_t& result);
void POMDOG_EXPORT Get(const ConstantBuffer& constantBuffer, std::uint32_t& result);
void POMDOG_EXPORT Get(const ConstantBuffer& constantBuffer, float& result);
void POMDOG_EXPORT Get(const ConstantBuffer& constantBuffer, double& result);

template <class T, class ConstantBuffer>
void Get(const ConstantBuffer& constantBuffer, T& result)
{
    static_assert(std::is_pod<T>::value, "You can only use POD value.");
    constantBuffer.GetValue(sizeof(result), static_cast<void*>(&result));
}

void POMDOG_EXPORT Set(ConstantBuffer& constantBuffer, bool value);
void POMDOG_EXPORT Set(ConstantBuffer& constantBuffer, std::int32_t value);
void POMDOG_EXPORT Set(ConstantBuffer& constantBuffer, std::uint32_t value);
void POMDOG_EXPORT Set(ConstantBuffer& constantBuffer, float value);
void POMDOG_EXPORT Set(ConstantBuffer& constantBuffer, double value);

//void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, const bool* data, std::uint32_t count);
void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, const std::int32_t* data, std::uint32_t count);
void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, const std::uint32_t* data, std::uint32_t count);
void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, const float* data, std::uint32_t count);
void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, const double* data, std::uint32_t count);

template <typename T> void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, const FloatingPointVector2<T>& value);
template <typename T> void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, const FloatingPointVector3<T>& value);
template <typename T> void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, const FloatingPointVector4<T>& value);
template <typename T> void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, const FloatingPointMatrix2x2<T>& value);
template <typename T> void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, const FloatingPointMatrix3x3<T>& value);
template <typename T> void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, const FloatingPointMatrix4x4<T>& value);
template <typename T> void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, const FloatingPointQuaternion<T>& value);

template <class T, class ConstantBuffer>
void Set(ConstantBuffer& constantBuffer, const T& value)
{
    static_assert(std::is_pod<T>::value, "You can only use POD value.");
    constantBuffer.SetValue(static_cast<const void*>(&value), sizeof(value));
}

} // namespace EffectBinaryParameter
} // namespace Detail
} // namespace Pomdog
