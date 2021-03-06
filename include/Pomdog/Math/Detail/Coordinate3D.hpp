// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {

/// Coordinate3D is a point in 3-dimensional space.
template <typename T>
class POMDOG_EXPORT Coordinate3D final {
public:
    static_assert(std::is_arithmetic<T>::value,
        "You can only use arithmetic types.");
    using value_type = T;

    T X, Y, Z;

public:
    // Constructors:
    Coordinate3D() noexcept = default;

    /// Constructs from T values.
    Coordinate3D(T x, T y, T z) noexcept;

    // Assignment operators:
    Coordinate3D& operator+=(const Coordinate3D&) noexcept;
    Coordinate3D& operator-=(const Coordinate3D&) noexcept;
    Coordinate3D& operator*=(T scaleFactor) noexcept;
    Coordinate3D& operator/=(T scaleFactor);

    // Unary operators:
    Coordinate3D operator+() const noexcept;
    Coordinate3D operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Coordinate3D operator+(const Coordinate3D&) const noexcept;
    [[nodiscard]] Coordinate3D operator-(const Coordinate3D&) const noexcept;
    [[nodiscard]] Coordinate3D operator*(const Coordinate3D&) const noexcept;
    [[nodiscard]] Coordinate3D operator/(const Coordinate3D&) const;
    [[nodiscard]] Coordinate3D operator*(T scaleFactor) const noexcept;
    [[nodiscard]] Coordinate3D operator/(T scaleFactor) const;

    [[nodiscard]] bool operator==(const Coordinate3D&) const noexcept;
    [[nodiscard]] bool operator!=(const Coordinate3D&) const noexcept;

    static Coordinate3D const Zero;
};

template <typename T>
Coordinate3D<T> const Coordinate3D<T>::Zero = {0, 0, 0};

template <typename T>
Coordinate3D<T> POMDOG_EXPORT
operator*(T scaleFactor, const Coordinate3D<T>& coordinate) noexcept;

template <typename T>
Coordinate3D<T> POMDOG_EXPORT
operator/(T scaleFactor, const Coordinate3D<T>& coordinate);

} // namespace Pomdog::Detail
