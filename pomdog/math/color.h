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
    std::uint8_t R;
    std::uint8_t G;
    std::uint8_t B;
    std::uint8_t A;

public:
    Color() noexcept;

    /// Constructs from unsigned integer values.
    Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) noexcept;

    explicit Color(const Vector3& vector);
    explicit Color(const Vector4& vector);

    [[nodiscard]] bool operator==(const Color&) const noexcept;
    [[nodiscard]] bool operator!=(const Color&) const noexcept;

    [[nodiscard]] Vector3 ToVector3() const noexcept;
    [[nodiscard]] Vector4 ToVector4() const noexcept;

    [[nodiscard]] std::uint32_t
    ToPackedValue() const noexcept;

    [[nodiscard]] static Color
    FromPackedValue(std::uint32_t packedValue);

    /// Returns the Color (255, 255, 255, 255).
    [[nodiscard]] static Color White() noexcept;

    /// Returns the Color (0, 0, 0, 255).
    [[nodiscard]] static Color Black() noexcept;

    /// Returns the Color (255, 0, 0, 255).
    [[nodiscard]] static Color Red() noexcept;

    /// Returns the Color (0, 255, 0, 255).
    [[nodiscard]] static Color Green() noexcept;

    /// Returns the Color (0, 0, 255, 255).
    [[nodiscard]] static Color Blue() noexcept;

    /// Returns the Color (255, 255, 0, 255).
    [[nodiscard]] static Color Yellow() noexcept;

    /// Returns the Color (100, 149 ,237, 255).
    [[nodiscard]] static Color CornflowerBlue() noexcept;

    /// Returns the Color (0, 0, 0, 0).
    [[nodiscard]] static Color TransparentBlack() noexcept;
};

} // namespace pomdog

namespace pomdog::math {

/// Multiplies a color by a scalar factor.
[[nodiscard]] Color POMDOG_EXPORT
Multiply(const Color& color, float factor);

/// Multiplies a color by another color.
[[nodiscard]] Color POMDOG_EXPORT
Multiply(const Color& source1, const Color& source2);

/// Performs a linear interpolation between two colors.
[[nodiscard]] Color POMDOG_EXPORT
Lerp(const Color& source1, const Color& source2, float amount);

/// Performs Hermite interpolation between two colors.
[[nodiscard]] Color POMDOG_EXPORT
SmoothStep(const Color& source1, const Color& source2, float amount);

} // namespace pomdog::math
