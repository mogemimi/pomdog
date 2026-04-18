// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {

/// F32Color represents a color using 32-bit floating-point values for each RGBA component.
/// Unlike the Color class which uses 8-bit unsigned integers, F32Color provides
/// higher precision and supports values outside the [0, 1] range for HDR rendering.
class POMDOG_EXPORT alignas(16) F32Color final {
public:
    f32 r;
    f32 g;
    f32 b;
    f32 a;

public:
    F32Color() noexcept;

    /// Constructs from floating-point values.
    F32Color(f32 r, f32 g, f32 b, f32 a) noexcept;

    // Assignment operators:
    F32Color& operator+=(const F32Color&) noexcept;
    F32Color& operator-=(const F32Color&) noexcept;
    F32Color& operator*=(f32) noexcept;
    F32Color& operator/=(f32) noexcept;

    // Unary operators:
    [[nodiscard]] F32Color operator+() const noexcept;
    [[nodiscard]] F32Color operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] F32Color operator+(const F32Color&) const noexcept;
    [[nodiscard]] F32Color operator-(const F32Color&) const noexcept;
    [[nodiscard]] F32Color operator*(const F32Color&) const noexcept;
    [[nodiscard]] F32Color operator/(const F32Color&) const noexcept;

    [[nodiscard]] F32Color operator*(f32 scaleFactor) const noexcept;
    [[nodiscard]] F32Color operator/(f32 scaleFactor) const noexcept;

    [[nodiscard]] bool operator==(const F32Color& other) const noexcept;
    [[nodiscard]] bool operator!=(const F32Color& other) const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] const f32* data() const noexcept;
};

} // namespace pomdog

namespace pomdog::math {

/// Returns the component-wise absolute value of a color.
[[nodiscard]] F32Color POMDOG_EXPORT
abs(F32Color v) noexcept;

} // namespace pomdog::math
