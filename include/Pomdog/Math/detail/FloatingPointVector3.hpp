// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_FLOATINGPOINTVECTOR3_D6230CBF_HPP
#define POMDOG_FLOATINGPOINTVECTOR3_D6230CBF_HPP

#include "ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstddef>
#include <cstdint>
#include <cfloat>
#include <type_traits>

namespace Pomdog {
namespace Detail {

///@brief vector in three-dimensional space.
template <typename T>
class POMDOG_EXPORT FloatingPointVector3 {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    typedef T value_type;

    T X, Y, Z;

public:
    // Constructors:
    FloatingPointVector3() noexcept = default;

    ///@brief Construct from floating-point values.
    FloatingPointVector3(T x, T y, T z) noexcept;

    FloatingPointVector3(FloatingPointVector2<T> const& vec, T z);

    // Assignment operators:
    FloatingPointVector3 & operator+=(FloatingPointVector3 const&);
    FloatingPointVector3 & operator-=(FloatingPointVector3 const&);
    FloatingPointVector3 & operator*=(T);
    FloatingPointVector3 & operator/=(T);

    // Unary operators:
    FloatingPointVector3 operator+() const;
    FloatingPointVector3 operator-() const;

    // Binary operators:
    FloatingPointVector3 operator+(FloatingPointVector3 const&) const;
    FloatingPointVector3 operator-(FloatingPointVector3 const&) const;
    FloatingPointVector3 operator*(FloatingPointVector3 const&) const;
    FloatingPointVector3 operator/(FloatingPointVector3 const&) const;
    FloatingPointVector3 operator*(T scaleFactor) const;
    FloatingPointVector3 operator/(T scaleFactor) const;

    bool operator==(FloatingPointVector3 const& other) const;
    bool operator!=(FloatingPointVector3 const& other) const;

    T Length() const;

    T LengthSquared() const;

    static T Distance(FloatingPointVector3 const& a, FloatingPointVector3 const& b);

    static T DistanceSquared(FloatingPointVector3 const& a, FloatingPointVector3 const& b);

    static T Dot(FloatingPointVector3 const& a, FloatingPointVector3 const& b);

    static FloatingPointVector3 Cross(FloatingPointVector3 const& a, FloatingPointVector3 const& b);

    static FloatingPointVector3 Min(FloatingPointVector3 const& a, FloatingPointVector3 const& b);

    static FloatingPointVector3 Max(FloatingPointVector3 const& a, FloatingPointVector3 const& b);

    static FloatingPointVector3 Clamp(FloatingPointVector3 const& source,
        FloatingPointVector3 const& min, FloatingPointVector3 const& max);

    static FloatingPointVector3 Lerp(FloatingPointVector3 const& source1, FloatingPointVector3 const& source2, T amount);

    static FloatingPointVector3 SmoothStep(FloatingPointVector3 const& source1, FloatingPointVector3 const& source2, T amount);

    void Normalize();

    static FloatingPointVector3 Normalize(FloatingPointVector3 const& source);

    static void Normalize(FloatingPointVector3 const&, FloatingPointVector3 & result);

    static FloatingPointVector3 Transform(FloatingPointVector3 const& position, FloatingPointMatrix4x4<T> const& matrix);

    static FloatingPointVector3 TransformNormal(FloatingPointVector3 const& normal, FloatingPointMatrix4x4<T> const& matrix);

    ///@brief Returns pointer to the first element.
    T const* Data() const noexcept;

    ///@brief Returns pointer to the first element.
    T* Data() noexcept;

    ///@note (0, 0, 0)
    static FloatingPointVector3 const Zero;

    ///@note (1, 0, 0)
    static FloatingPointVector3 const UnitX;

    ///@note (0, 1, 0)
    static FloatingPointVector3 const UnitY;

    ///@note (0, 0, 1)
    static FloatingPointVector3 const UnitZ;
};

template <typename T>
FloatingPointVector3<T> POMDOG_EXPORT
operator*(T scaleFactor, FloatingPointVector3<T> const& vector);

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_FLOATINGPOINTVECTOR3_D6230CBF_HPP
