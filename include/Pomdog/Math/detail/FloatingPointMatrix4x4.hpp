// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstddef>
#include <cstdint>
#include <cfloat>
#include <type_traits>
#include <array>

namespace Pomdog {
namespace Detail {

template <typename T>
class POMDOG_EXPORT FloatingPointMatrix4x4 {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    typedef T value_type;

    //union {
    //    std::array<std::array<T, 4>, 4> m;
    //#if defined __i386__ || defined __x86_64__
    //    std::array<__m128, 4>
    //#endif
    //};
    std::array<std::array<T, 4>, 4> m;

private:
    static constexpr std::size_t RowSize = 4;
    static constexpr std::size_t ColumnSize = 4;

public:
    // Constructors:
    FloatingPointMatrix4x4() noexcept = default;

    ///@brief Construct from floating-point values.
    FloatingPointMatrix4x4(
        T m00, T m01, T m02, T m03,
        T m10, T m11, T m12, T m13,
        T m20, T m21, T m22, T m23,
        T m30, T m31, T m32, T m33) noexcept;

    // Assignment operators:
    FloatingPointMatrix4x4 & operator*=(FloatingPointMatrix4x4 const&) noexcept;
    FloatingPointMatrix4x4 & operator+=(FloatingPointMatrix4x4 const&) noexcept;
    FloatingPointMatrix4x4 & operator-=(FloatingPointMatrix4x4 const&) noexcept;
    FloatingPointMatrix4x4 & operator*=(T scaleFactor) noexcept;
    FloatingPointMatrix4x4 & operator/=(T scaleFactor);

    // Unary operators:
    FloatingPointMatrix4x4 operator+() const noexcept;
    FloatingPointMatrix4x4 operator-() const noexcept;

    // Binary operators:
    FloatingPointMatrix4x4 operator+(FloatingPointMatrix4x4 const&) const noexcept;
    FloatingPointMatrix4x4 operator-(FloatingPointMatrix4x4 const&) const noexcept;
    FloatingPointMatrix4x4 operator*(FloatingPointMatrix4x4 const&) const noexcept;
    FloatingPointMatrix4x4 operator*(T scaleFactor) const noexcept;
    FloatingPointMatrix4x4 operator/(T scaleFactor) const;

    bool operator==(FloatingPointMatrix4x4 const&) const noexcept;
    bool operator!=(FloatingPointMatrix4x4 const&) const noexcept;

    // Function-call operators:
    T const& operator()(std::size_t row, std::size_t column) const;
    T & operator()(std::size_t row, std::size_t column);

    void SetTranslation(FloatingPointVector3<T> const&) noexcept;

    FloatingPointVector3<T> GetTranslation() const noexcept;

    void SetScale(FloatingPointVector3<T> const&) noexcept;

    FloatingPointVector3<T> GetScale() const noexcept;

    T Determinant() const noexcept;

    FloatingPointMatrix4x4 Concatenate(FloatingPointMatrix4x4 const&) const noexcept;

    FloatingPointMatrix4x4 Concatenate(T scaleFactor) const noexcept;

    FloatingPointMatrix3x3<T> Minor3x3(std::size_t row, std::size_t column) const;

    static FloatingPointMatrix4x4
    Adjoint(FloatingPointMatrix4x4 const& matrix);

    static void
    Transpose(FloatingPointMatrix4x4 const& matrix, FloatingPointMatrix4x4 & result) noexcept;

    static FloatingPointMatrix4x4
    Transpose(FloatingPointMatrix4x4 const& matrix) noexcept;

    static void
    Invert(FloatingPointMatrix4x4 const& matrix, FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    Invert(FloatingPointMatrix4x4 const& matrix);

    static void
    Lerp(FloatingPointMatrix4x4 const& source1, FloatingPointMatrix4x4 const& source2, T amount,
        FloatingPointMatrix4x4 & result) noexcept;

    static FloatingPointMatrix4x4
    Lerp(FloatingPointMatrix4x4 const& source1, FloatingPointMatrix4x4 const& source2, T amount) noexcept;

    static void
    CreateTranslation(FloatingPointVector3<T> const& position, FloatingPointMatrix4x4 & result) noexcept;

    static FloatingPointMatrix4x4
    CreateTranslation(FloatingPointVector3<T> const& position) noexcept;

    static void
    CreateScale(T scale, FloatingPointMatrix4x4 & result) noexcept;

    static FloatingPointMatrix4x4
    CreateScale(T scale) noexcept;

    static void
    CreateScale(FloatingPointVector3<T> const& scale, FloatingPointMatrix4x4 & result) noexcept;

    static FloatingPointMatrix4x4
    CreateScale(FloatingPointVector3<T> const& scale) noexcept;

    static void
    CreateRotationX(Radian<T> const& angle, FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    CreateRotationX(Radian<T> const& angle);

    static void
    CreateRotationY(Radian<T> const& angle, FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    CreateRotationY(Radian<T> const& angle);

    static void
    CreateRotationZ(Radian<T> const& angle, FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    CreateRotationZ(Radian<T> const& angle);

    static void
    CreateFromQuaternion(FloatingPointQuaternion<T> const& quaternion, FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    CreateFromQuaternion(FloatingPointQuaternion<T> const& quaternion);

    static void
    CreateLookAtLH(FloatingPointVector3<T> const& eye, FloatingPointVector3<T> const& at,
        FloatingPointVector3<T> const& up, FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    CreateLookAtLH(FloatingPointVector3<T> const& eye, FloatingPointVector3<T> const& at,
        FloatingPointVector3<T> const& up);

    static void
    CreateLookAtRH(FloatingPointVector3<T> const& eye, FloatingPointVector3<T> const& at,
        FloatingPointVector3<T> const& up, FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    CreateLookAtRH(FloatingPointVector3<T> const& eye, FloatingPointVector3<T> const& at,
        FloatingPointVector3<T> const& up);

    static void
    CreatePerspectiveLH(T width, T height, T zNear, T zFar, FloatingPointMatrix4x4 & result);

    static void
    CreatePerspectiveRH(T width, T height, T zNear, T zFar, FloatingPointMatrix4x4 & result);

    static void
    CreatePerspectiveFieldOfViewLH(Radian<T> const& fovy, T aspect, T zNear, T zFar,
        FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    CreatePerspectiveFieldOfViewLH(Radian<T> const& fovy, T aspect, T zNear, T zFar);

    static void
    CreatePerspectiveFieldOfViewRH(Radian<T> const& fovy, T aspect, T zNear, T zFar,
        FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    CreatePerspectiveFieldOfViewRH(Radian<T> const& fovy, T aspect, T zNear, T zFar);

    static void
    CreatePerspectiveOffCenterLH(T left, T right, T bottom, T top, T zNear, T zFar,
        FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    CreatePerspectiveOffCenterLH(T left, T right, T bottom, T top, T zNear, T zFar);

    static void
    CreatePerspectiveOffCenterRH(T left, T right, T bottom, T top, T zNear, T zFar,
        FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    CreatePerspectiveOffCenterRH(T left, T right, T bottom, T top, T zNear, T zFar);

    static void
    CreateOrthographicLH(T width, T height, T zNearPlane, T zFarPlane,
        FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    CreateOrthographicLH(T width, T height, T zNearPlane, T zFarPlane);

    static void
    CreateOrthographicRH(T width, T height, T zNearPlane, T zFarPlane,
        FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    CreateOrthographicRH(T width, T height, T zNearPlane, T zFarPlane);

    static void
    CreateOrthographicOffCenterLH(T left, T right, T bottom, T top, T zNearPlane, T zFarPlane,
        FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    CreateOrthographicOffCenterLH(T left, T right, T bottom, T top, T zNearPlane, T zFarPlane);

    static void
    CreateOrthographicOffCenterRH(T left, T right, T bottom, T top, T zNearPlane, T zFarPlane,
        FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    CreateOrthographicOffCenterRH(T left, T right, T bottom, T top, T zNearPlane, T zFarPlane);

    static void
    CreateFromAxisAngle(FloatingPointVector3<T> const& axis, Radian<T> const& angle, FloatingPointMatrix4x4 & result);

    static FloatingPointMatrix4x4
    CreateFromAxisAngle(FloatingPointVector3<T> const& axis, Radian<T> const& angle);

    ///@brief Returns pointer to the first element.
    T const* Data() const noexcept;

    ///@brief Returns pointer to the first element.
    T* Data() noexcept;

    static FloatingPointMatrix4x4 const Identity;
};

template <typename T>
FloatingPointMatrix4x4<T> POMDOG_EXPORT
operator*(T scaleFactor, FloatingPointMatrix4x4<T> const& matrix) noexcept;

} // namespace Detail
} // namespace Pomdog
