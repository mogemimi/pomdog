// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include <type_traits>

namespace Pomdog::Detail {

template <typename T>
class POMDOG_EXPORT Coordinate2D final {
public:
    static_assert(std::is_arithmetic<T>::value,
        "You can only use arithmetic types.");
    typedef T value_type;

    T X, Y;

public:
    // Constructors:
    Coordinate2D() noexcept = default;

    /// Construct from T values.
    Coordinate2D(T x, T y) noexcept;

    // Assignment operators:
    Coordinate2D& operator+=(const Coordinate2D&) noexcept;
    Coordinate2D& operator-=(const Coordinate2D&) noexcept;
    Coordinate2D& operator*=(T scaleFactor) noexcept;
    Coordinate2D& operator/=(T scaleFactor);

    // Unary operators:
    Coordinate2D operator+() const noexcept;
    Coordinate2D operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Coordinate2D operator+(const Coordinate2D&) const noexcept;
    [[nodiscard]] Coordinate2D operator-(const Coordinate2D&) const noexcept;
    [[nodiscard]] Coordinate2D operator*(const Coordinate2D&) const noexcept;
    [[nodiscard]] Coordinate2D operator/(const Coordinate2D&) const;
    [[nodiscard]] Coordinate2D operator*(T scaleFactor) const noexcept;
    [[nodiscard]] Coordinate2D operator/(T scaleFactor) const;

    [[nodiscard]] bool operator==(const Coordinate2D&) const noexcept;
    [[nodiscard]] bool operator!=(const Coordinate2D&) const noexcept;

    static Coordinate2D const Zero;
};

template <typename T>
Coordinate2D<T> const Coordinate2D<T>::Zero = {0, 0};

template <typename T>
Coordinate2D<T> POMDOG_EXPORT
operator*(T scaleFactor, const Coordinate2D<T>& coordinate) noexcept;

template <typename T>
Coordinate2D<T> POMDOG_EXPORT
operator/(T scaleFactor, const Coordinate2D<T>& coordinate);

} // namespace Pomdog::Detail
