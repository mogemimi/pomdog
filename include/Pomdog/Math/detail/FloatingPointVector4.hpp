// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstddef>
#include <cstdint>
#include <cfloat>
#include <type_traits>

namespace Pomdog {
namespace Detail {

///@brief vector in four-dimensional space.
template <typename T>
class POMDOG_EXPORT FloatingPointVector4 {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    typedef T value_type;

    T X, Y, Z, W;

public:
    // Constructors:
    FloatingPointVector4() noexcept = default;

    ///@brief Construct from floating-point values.
    FloatingPointVector4(T x, T y, T z, T w) noexcept;
    FloatingPointVector4(FloatingPointVector3<T> const& xyz, T w) noexcept;

    // Assignment operators:
    FloatingPointVector4 & operator+=(FloatingPointVector4 const& other) noexcept;
    FloatingPointVector4 & operator-=(FloatingPointVector4 const& other) noexcept;
    FloatingPointVector4 & operator*=(T scaleFactor) noexcept;
    FloatingPointVector4 & operator/=(T scaleFactor);

    // Unary operators:
    FloatingPointVector4 operator+() const noexcept;
    FloatingPointVector4 operator-() const noexcept;

    // Binary operators:
    FloatingPointVector4 operator+(FloatingPointVector4 const& other) const noexcept;
    FloatingPointVector4 operator-(FloatingPointVector4 const& other) const noexcept;
    FloatingPointVector4 operator*(FloatingPointVector4 const& other) const noexcept;
    FloatingPointVector4 operator/(FloatingPointVector4 const& other) const;
    FloatingPointVector4 operator*(T scaleFactor) const noexcept;
    FloatingPointVector4 operator/(T scaleFactor) const;

    bool operator==(FloatingPointVector4 const& other) const noexcept;
    bool operator!=(FloatingPointVector4 const& other) const noexcept;

    T Length() const noexcept;

    T LengthSquared() const noexcept;

    static T Distance(FloatingPointVector4 const& a, FloatingPointVector4 const& b) noexcept;

    static T DistanceSquared(FloatingPointVector4 const& a, FloatingPointVector4 const& b) noexcept;

    static T Dot(FloatingPointVector4 const& a, FloatingPointVector4 const& b) noexcept;

    void Normalize() noexcept;

    static FloatingPointVector4 Normalize(FloatingPointVector4 const& source) noexcept;

    static void Normalize(FloatingPointVector4 const& source, FloatingPointVector4 & result) noexcept;

    ///@brief Returns pointer to the first element.
    T const* Data() const noexcept;

    ///@brief Returns pointer to the first element.
    T* Data() noexcept;
};

template <typename T>
FloatingPointVector4<T> POMDOG_EXPORT
operator*(T scaleFactor, FloatingPointVector4<T> const& vector) noexcept;

} // namespace Detail
} // namespace Pomdog
