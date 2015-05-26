// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTBINARYPARAMETER_F5EA7DB2_HPP
#define POMDOG_EFFECTBINARYPARAMETER_F5EA7DB2_HPP

#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <type_traits>

namespace Pomdog {

class ConstantBuffer;

namespace Detail {
namespace EffectBinaryParameter {

void POMDOG_EXPORT Get(ConstantBuffer const& constantBuffer, bool & result);
void POMDOG_EXPORT Get(ConstantBuffer const& constantBuffer, std::int32_t & result);
void POMDOG_EXPORT Get(ConstantBuffer const& constantBuffer, std::uint32_t & result);
void POMDOG_EXPORT Get(ConstantBuffer const& constantBuffer, float & result);
void POMDOG_EXPORT Get(ConstantBuffer const& constantBuffer, double & result);

template <class T, class ConstantBuffer>
void Get(ConstantBuffer const& constantBuffer, T & result)
{
    static_assert(std::is_pod<T>::value, "You can only use POD value.");
    constantBuffer.GetValue(sizeof(result), static_cast<void*>(&result));
}

void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, bool value);
void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, std::int32_t value);
void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, std::uint32_t value);
void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, float value);
void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, double value);

//void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, bool const* data, std::uint32_t count);
void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, std::int32_t const* data, std::uint32_t count);
void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, std::uint32_t const* data, std::uint32_t count);
void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, float const* data, std::uint32_t count);
void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, double const* data, std::uint32_t count);

template <typename T> void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, FloatingPointVector2<T> const& value);
template <typename T> void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, FloatingPointVector3<T> const& value);
template <typename T> void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, FloatingPointVector4<T> const& value);
template <typename T> void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, FloatingPointMatrix2x2<T> const& value);
template <typename T> void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, FloatingPointMatrix3x3<T> const& value);
template <typename T> void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, FloatingPointMatrix4x4<T> const& value);
template <typename T> void POMDOG_EXPORT Set(ConstantBuffer & constantBuffer, FloatingPointQuaternion<T> const& value);

template <class T, class ConstantBuffer>
void Set(ConstantBuffer & constantBuffer, T const& value)
{
    static_assert(std::is_pod<T>::value, "You can only use POD value.");
    constantBuffer.SetValue(static_cast<void const*>(&value), sizeof(value));
}

} // namespace EffectBinaryParameter
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_EFFECTBINARYPARAMETER_F5EA7DB2_HPP
