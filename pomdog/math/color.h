// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/math/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Color describes a 32-bit packed RGBA color.
class POMDOG_EXPORT Color final {
public:
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;

public:
    Color() noexcept;

    /// Constructs from unsigned integer values.
    Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) noexcept;

    explicit Color(const Vector3& vector);
    explicit Color(const Vector4& vector);

    [[nodiscard]] bool operator==(const Color&) const noexcept;
    [[nodiscard]] bool operator!=(const Color&) const noexcept;

    [[nodiscard]] Vector3
    toVector3() const noexcept;

    [[nodiscard]] Vector4
    toVector4() const noexcept;

    [[nodiscard]] std::uint32_t
    toPackedValue() const noexcept;

    [[nodiscard]] static Color
    fromPackedValue(std::uint32_t packedValue) noexcept;

    /// Returns the Color (255, 255, 255, 255).
    [[nodiscard]] static Color createWhite() noexcept;

    /// Returns the Color (0, 0, 0, 255).
    [[nodiscard]] static Color createBlack() noexcept;

    /// Returns the Color (255, 0, 0, 255).
    [[nodiscard]] static Color createRed() noexcept;

    /// Returns the Color (0, 255, 0, 255).
    [[nodiscard]] static Color createGreen() noexcept;

    /// Returns the Color (0, 0, 255, 255).
    [[nodiscard]] static Color createBlue() noexcept;

    /// Returns the Color (255, 255, 0, 255).
    [[nodiscard]] static Color createYellow() noexcept;

    /// Returns the Color (100, 149 ,237, 255).
    [[nodiscard]] static Color createCornflowerBlue() noexcept;

    /// Returns the Color (0, 0, 0, 0).
    [[nodiscard]] static Color createTransparentBlack() noexcept;
};

} // namespace pomdog

namespace pomdog::math {

/// Multiplies a color by a scalar factor.
[[nodiscard]] Color POMDOG_EXPORT
multiply(const Color& color, float factor);

/// Multiplies a color by another color.
[[nodiscard]] Color POMDOG_EXPORT
multiply(const Color& source1, const Color& source2);

/// Performs a linear interpolation between two colors.
[[nodiscard]] Color POMDOG_EXPORT
lerp(const Color& source1, const Color& source2, float amount);

/// Performs Hermite interpolation between two colors.
[[nodiscard]] Color POMDOG_EXPORT
smoothstep(const Color& source1, const Color& source2, float amount);

} // namespace pomdog::math
