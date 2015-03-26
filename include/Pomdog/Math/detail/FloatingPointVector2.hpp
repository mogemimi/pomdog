// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_FLOATINGPOINTVECTOR2_714DEF9F_HPP
#define POMDOG_FLOATINGPOINTVECTOR2_714DEF9F_HPP

#include "ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstddef>
#include <cstdint>
#include <cfloat>
#include <type_traits>

namespace Pomdog {
namespace Detail {

///@brief vector in two-dimensional space.
template <typename T>
class POMDOG_EXPORT FloatingPointVector2 {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    typedef T value_type;

    T X, Y;

public:
    // Constructors:
    FloatingPointVector2() noexcept = default;

    ///@brief Construct from floating-point values.
    FloatingPointVector2(T x, T y) noexcept;

    // Assignment operators:
    FloatingPointVector2 & operator+=(FloatingPointVector2 const&);
    FloatingPointVector2 & operator-=(FloatingPointVector2 const&);
    FloatingPointVector2 & operator*=(T);
    FloatingPointVector2 & operator/=(T);

    // Unary operators:
    FloatingPointVector2 operator+() const;
    FloatingPointVector2 operator-() const;

    // Binary operators:
    FloatingPointVector2 operator+(FloatingPointVector2 const&) const;
    FloatingPointVector2 operator-(FloatingPointVector2 const&) const;
    FloatingPointVector2 operator*(FloatingPointVector2 const&) const;
    FloatingPointVector2 operator/(FloatingPointVector2 const&) const;

    FloatingPointVector2 operator*(T scaleFactor) const;
    FloatingPointVector2 operator/(T scaleFactor) const;

    bool operator==(FloatingPointVector2 const& other) const;
    bool operator!=(FloatingPointVector2 const& other) const;

    T Length() const;

    T LengthSquared() const;

    static T Distance(FloatingPointVector2 const& a, FloatingPointVector2 const& b);

    static T DistanceSquared(FloatingPointVector2 const& a, FloatingPointVector2 const& b);

    static T Dot(FloatingPointVector2 const& a, FloatingPointVector2 const& b);

    static T Cross(FloatingPointVector2 const& a, FloatingPointVector2 const& b);

    static FloatingPointVector2 Min(FloatingPointVector2 const& a, FloatingPointVector2 const& b);

    static FloatingPointVector2 Max(FloatingPointVector2 const& a, FloatingPointVector2 const& b);

    static FloatingPointVector2 Clamp(FloatingPointVector2 const& source,
        FloatingPointVector2 const& min, FloatingPointVector2 const& max);

    void Normalize();

    static FloatingPointVector2 Normalize(FloatingPointVector2 const& source);

    static void Normalize(FloatingPointVector2 const& source, FloatingPointVector2 & result);

    static FloatingPointVector2 Transform(FloatingPointVector2 const& position, FloatingPointMatrix3x2<T> const& matrix);

    static FloatingPointVector2 Transform(FloatingPointVector2 const& position, FloatingPointMatrix4x4<T> const& matrix);

    static FloatingPointVector2 Transform(FloatingPointVector2 const& position, FloatingPointQuaternion<T> const& rotation);

    static FloatingPointVector2 TransformNormal(FloatingPointVector2 const& normal, FloatingPointMatrix4x4<T> const& matrix);

    static FloatingPointVector2 Lerp(FloatingPointVector2 const& source1, FloatingPointVector2 const& source2, T amount);

    static FloatingPointVector2 SmoothStep(FloatingPointVector2 const& source1, FloatingPointVector2 const& source2, T amount);

    ///@brief Returns pointer to the first element.
    T const* Data() const noexcept;

    ///@brief Returns pointer to the first element.
    T* Data() noexcept;

    ///@note (0, 0, 0)
    static FloatingPointVector2 const Zero;
};

template <typename T>
FloatingPointVector2<T> POMDOG_EXPORT
operator*(T scaleFactor, FloatingPointVector2<T> const& vector);

}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_FLOATINGPOINTVECTOR2_714DEF9F_HPP
