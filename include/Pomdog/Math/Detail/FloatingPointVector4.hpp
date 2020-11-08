// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/ForwardDeclarations.hpp"
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Pomdog::Detail {

///@brief vector in four-dimensional space.
template <typename T>
class POMDOG_EXPORT FloatingPointVector4 final {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    using value_type = T;

    T X, Y, Z, W;

public:
    // Constructors:
    FloatingPointVector4() noexcept = default;

    ///@brief Construct from floating-point values.
    FloatingPointVector4(T x, T y, T z, T w) noexcept;
    FloatingPointVector4(const FloatingPointVector3<T>& xyz, T w) noexcept;

    // Assignment operators:
    FloatingPointVector4& operator+=(const FloatingPointVector4& other) noexcept;
    FloatingPointVector4& operator-=(const FloatingPointVector4& other) noexcept;
    FloatingPointVector4& operator*=(T scaleFactor) noexcept;
    FloatingPointVector4& operator/=(T scaleFactor);

    // Unary operators:
    FloatingPointVector4 operator+() const noexcept;
    FloatingPointVector4 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] FloatingPointVector4 operator+(const FloatingPointVector4& other) const noexcept;
    [[nodiscard]] FloatingPointVector4 operator-(const FloatingPointVector4& other) const noexcept;
    [[nodiscard]] FloatingPointVector4 operator*(const FloatingPointVector4& other) const noexcept;
    [[nodiscard]] FloatingPointVector4 operator/(const FloatingPointVector4& other) const;
    [[nodiscard]] FloatingPointVector4 operator*(T scaleFactor) const noexcept;
    [[nodiscard]] FloatingPointVector4 operator/(T scaleFactor) const;

    [[nodiscard]] bool operator==(const FloatingPointVector4& other) const noexcept;
    [[nodiscard]] bool operator!=(const FloatingPointVector4& other) const noexcept;

    [[nodiscard]] T Length() const noexcept;

    [[nodiscard]] T LengthSquared() const noexcept;

    [[nodiscard]] static T
    Distance(const FloatingPointVector4& a, const FloatingPointVector4& b) noexcept;

    [[nodiscard]] static T
    DistanceSquared(const FloatingPointVector4& a, const FloatingPointVector4& b) noexcept;

    [[nodiscard]] static T
    Dot(const FloatingPointVector4& a, const FloatingPointVector4& b) noexcept;

    void Normalize() noexcept;

    [[nodiscard]] static FloatingPointVector4
    Normalize(const FloatingPointVector4& source) noexcept;

    static void Normalize(const FloatingPointVector4& source, FloatingPointVector4& result) noexcept;

    [[nodiscard]] static FloatingPointVector4
    Transform(
        const FloatingPointVector4& position,
        const FloatingPointMatrix4x4<T>& matrix) noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] const T* Data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] T* Data() noexcept;
};

template <typename T>
FloatingPointVector4<T> POMDOG_EXPORT
operator*(T scaleFactor, const FloatingPointVector4<T>& vector) noexcept;

} // namespace Pomdog::Detail
