// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX3X2_450DA908_CC1A_4EAC_9739_7E22E8453CDD_HPP
#define POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX3X2_450DA908_CC1A_4EAC_9739_7E22E8453CDD_HPP

#include "ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstddef>
#include <cstdint>
#include <cfloat>
#include <type_traits>
#include <array>

namespace Pomdog {
namespace Detail {

template <typename T>
class POMDOG_EXPORT FloatingPointMatrix3x2 {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    typedef T value_type;

    std::array<std::array<T, 2>, 3> m;

private:
    static constexpr std::size_t RowSize = 3;
    static constexpr std::size_t ColumnSize = 2;

public:
    // Constructors:
    FloatingPointMatrix3x2() noexcept = default;

    ///@brief Construct from floating-point values.
    FloatingPointMatrix3x2(
        T m00, T m01,
        T m10, T m11,
        T m20, T m21) noexcept;

    // Assignment operators:
    FloatingPointMatrix3x2 & operator*=(FloatingPointMatrix3x2 const& other);
    FloatingPointMatrix3x2 & operator+=(FloatingPointMatrix3x2 const& other);
    FloatingPointMatrix3x2 & operator-=(FloatingPointMatrix3x2 const& other);
    FloatingPointMatrix3x2 & operator*=(T scaleFactor);
    FloatingPointMatrix3x2 & operator/=(T scaleFactor);

    // Unary operators:
    FloatingPointMatrix3x2 operator+() const;
    FloatingPointMatrix3x2 operator-() const;

    // Binary operators:
    FloatingPointMatrix3x2 operator+(FloatingPointMatrix3x2 const& other) const;
    FloatingPointMatrix3x2 operator-(FloatingPointMatrix3x2 const& other) const;
    FloatingPointMatrix3x2 operator*(FloatingPointMatrix3x2 const& other) const;
    FloatingPointMatrix3x2 operator*(T scaleFactor) const;
    FloatingPointMatrix3x2 operator/(T scaleFactor) const;

    bool operator==(FloatingPointMatrix3x2 const& other) const;
    bool operator!=(FloatingPointMatrix3x2 const& other) const;

    // Function-call operators:
    T const& operator()(std::size_t row, std::size_t column) const;
    T & operator()(std::size_t row, std::size_t column);

    T Determinant() const;

    FloatingPointMatrix3x2 Concatenate(FloatingPointMatrix3x2 const& other) const;

    FloatingPointMatrix3x2 Concatenate(T scaleFactor) const;

    static void
    Invert(FloatingPointMatrix3x2 const& matrix, FloatingPointMatrix3x2 & result);

    static FloatingPointMatrix3x2
    Invert(FloatingPointMatrix3x2 const& matrix);

    static void
    CreateTranslation(FloatingPointVector2<T> const& position, FloatingPointMatrix3x2 & result);

    static FloatingPointMatrix3x2
    CreateTranslation(FloatingPointVector2<T> const& position);

    static void
    CreateScale(T scale, FloatingPointMatrix3x2 & result);

    static FloatingPointMatrix3x2
    CreateScale(T scale);

    static void
    CreateScale(FloatingPointVector2<T> const& scale, FloatingPointMatrix3x2 & result);

    static FloatingPointMatrix3x2
    CreateScale(FloatingPointVector2<T> const& scale);

    static void
    CreateRotation(Radian<T> const& angle, FloatingPointMatrix3x2 & result);

    static FloatingPointMatrix3x2
    CreateRotation(Radian<T> const& angle);

    static void
    CreateSkew(FloatingPointVector2<T> const& skew, FloatingPointMatrix3x2 & result);

    static FloatingPointMatrix3x2
    CreateSkew(FloatingPointVector2<T> const& skew);

    static void
    CreateLerp(FloatingPointMatrix3x2 const& source1, FloatingPointMatrix3x2 const& source2,
        T amount, FloatingPointMatrix3x2 & result);

    static FloatingPointMatrix3x2
    CreateLerp(FloatingPointMatrix3x2 const& source1, FloatingPointMatrix3x2 const& source2,
        T amount);

    ///@brief Returns pointer to the first element.
    T const* Data() const noexcept;

    ///@brief Returns pointer to the first element.
    T* Data() noexcept;

    static FloatingPointMatrix3x2 const Identity;
};

template <typename T>
FloatingPointMatrix3x2<T> POMDOG_EXPORT
operator*(T scaleFactor, FloatingPointMatrix3x2<T> const& matrix);

}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX3X2_450DA908_CC1A_4EAC_9739_7E22E8453CDD_HPP)
