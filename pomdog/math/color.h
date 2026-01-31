// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
class Vector3;
class Vector4;
} // namespace pomdog

namespace pomdog {

/// Color represents a 32-bit packed RGBA color.
class POMDOG_EXPORT alignas(alignof(u32)) Color final {
public:
    u8 r;
    u8 g;
    u8 b;
    u8 a;

public:
    Color() noexcept;

    /// Constructs from unsigned integer values.
    Color(u8 r, u8 g, u8 b, u8 a) noexcept;

    explicit Color(const Vector3& vector);
    explicit Color(const Vector4& vector);

    [[nodiscard]] bool operator==(const Color& other) const noexcept;
    [[nodiscard]] bool operator!=(const Color& other) const noexcept;

    /// Converts to Vector3.
    [[nodiscard]] Vector3
    toVector3() const noexcept;

    /// Converts to Vector4.
    [[nodiscard]] Vector4
    toVector4() const noexcept;

    /// Converts to a packed 32-bit unsigned integer value.
    [[nodiscard]] u32
    toPackedValue() const noexcept;

    /// Creates a Color from a packed 32-bit unsigned integer value.
    [[nodiscard]] static Color
    fromPackedValue(u32 packedValue) noexcept;

    /// Returns a white color (RGBA: 255, 255, 255, 255).
    [[nodiscard]] static Color createWhite() noexcept;

    /// Returns a black color (RGBA: 0, 0, 0, 255).
    [[nodiscard]] static Color createBlack() noexcept;

    /// Returns a red color (RGBA: 255, 0, 0, 255).
    [[nodiscard]] static Color createRed() noexcept;

    /// Returns a green color (RGBA: 0, 255, 0, 255).
    [[nodiscard]] static Color createGreen() noexcept;

    /// Returns a blue color (RGBA: 0, 0, 255, 255).
    [[nodiscard]] static Color createBlue() noexcept;

    /// Returns a yellow color (RGBA: 255, 255, 0, 255).
    [[nodiscard]] static Color createYellow() noexcept;

    /// Returns a cornflower blue color (RGBA: 100, 149, 237, 255).
    [[nodiscard]] static Color createCornflowerBlue() noexcept;

    /// Returns a transparent black color (RGBA: 0, 0, 0, 0).
    [[nodiscard]] static Color createTransparentBlack() noexcept;
};

} // namespace pomdog

namespace pomdog::math {

/// Multiplies a color by a scalar factor.
[[nodiscard]] Color POMDOG_EXPORT
multiply(const Color& color, f32 factor);

/// Multiplies two colors component-wise.
[[nodiscard]] Color POMDOG_EXPORT
multiply(const Color& source1, const Color& source2);

/// Performs a linear interpolation between two colors.
[[nodiscard]] Color POMDOG_EXPORT
lerp(const Color& source1, const Color& source2, f32 amount);

/// Performs Hermite interpolation between two colors.
[[nodiscard]] Color POMDOG_EXPORT
smoothstep(const Color& source1, const Color& source2, f32 amount);

} // namespace pomdog::math
