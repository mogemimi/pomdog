// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstddef>
#include <cstdint>
#include <cfloat>
#include <type_traits>

namespace Pomdog {
namespace Detail {

template <typename T>
class POMDOG_EXPORT FloatingPointQuaternion {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    typedef T value_type;

    T X, Y, Z, W;

public:
    // Constructors:
    FloatingPointQuaternion() noexcept = default;

    ///@brief Construct from floating-point values.
    FloatingPointQuaternion(T x, T y, T z, T w) noexcept;

    // Assignment operators:
    FloatingPointQuaternion & operator+=(FloatingPointQuaternion const&);
    FloatingPointQuaternion & operator-=(FloatingPointQuaternion const&);
    FloatingPointQuaternion & operator*=(FloatingPointQuaternion const&);
    FloatingPointQuaternion & operator*=(T scaleFactor);
    FloatingPointQuaternion & operator/=(T scaleFactor);

    // Unary operators:
    FloatingPointQuaternion operator+() const;
    FloatingPointQuaternion operator-() const;

    // Binary operators:
    FloatingPointQuaternion operator+(FloatingPointQuaternion const&) const;
    FloatingPointQuaternion operator-(FloatingPointQuaternion const&) const;
    FloatingPointQuaternion operator*(FloatingPointQuaternion const&) const;
    FloatingPointQuaternion operator*(T scaleFactor) const;
    FloatingPointQuaternion operator/(T scaleFactor) const;

    bool operator==(FloatingPointQuaternion const&) const;
    bool operator!=(FloatingPointQuaternion const&) const;

    T Length() const;

    T LengthSquared() const;

    static T Dot(FloatingPointQuaternion const& a, FloatingPointQuaternion const& b);

    void Normalize();

    static FloatingPointQuaternion Normalize(FloatingPointQuaternion const& quaternion);

    static void Normalize(FloatingPointQuaternion const& quaternion, FloatingPointQuaternion & result);

    static void
    Slerp(FloatingPointQuaternion const& begin, FloatingPointQuaternion const& end, T amount, FloatingPointQuaternion & result);

    static FloatingPointQuaternion
    Slerp(FloatingPointQuaternion const& begin, FloatingPointQuaternion const& end, T amount);

    static void
    Inverse(FloatingPointQuaternion const& source, FloatingPointQuaternion & result);

    static FloatingPointQuaternion
    Inverse(FloatingPointQuaternion const& source);

    static void
    CreateFromAxisAngle(FloatingPointVector3<T> const& axis, Radian<T> const& angle, FloatingPointQuaternion & result);

    static FloatingPointQuaternion
    CreateFromAxisAngle(FloatingPointVector3<T> const& axis, Radian<T> const& angle);

    ///@brief Convert rotation matrix to quaternion.
    static void
    CreateFromRotationMatrix(FloatingPointMatrix4x4<T> const& rotation, FloatingPointQuaternion & result);

    ///@brief Convert rotation matrix to quaternion.
    static FloatingPointQuaternion
    CreateFromRotationMatrix(FloatingPointMatrix4x4<T> const& rotation);

    ///@brief Convert rotation matrix to quaternion.
    static void
    CreateFromRotationMatrix(FloatingPointMatrix3x3<T> const& rotation, FloatingPointQuaternion & result);

    ///@brief Convert rotation matrix to quaternion.
    static FloatingPointQuaternion
    CreateFromRotationMatrix(FloatingPointMatrix3x3<T> const& rotation);

    static void
    CreateFromYawPitchRoll(Radian<T> const& yaw, Radian<T> const& pitch, Radian<T> const& roll, FloatingPointQuaternion & result);

    static FloatingPointQuaternion
    CreateFromYawPitchRoll(Radian<T> const& yaw, Radian<T> const& pitch, Radian<T> const& roll);

    ///@brief Returns pointer to the first element.
    T const* Data() const noexcept;

    ///@brief Returns pointer to the first element.
    T* Data() noexcept;

    static FloatingPointQuaternion const Identity;
};

template <typename T>
FloatingPointQuaternion<T> POMDOG_EXPORT
operator*(T scaleFactor, FloatingPointQuaternion<T> const& quaternion);

} // namespace Detail
} // namespace Pomdog
