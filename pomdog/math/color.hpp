// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/math/forward_declarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

/// Color describes a 32-bit packed RGBA color.
class POMDOG_EXPORT Color final {
public:
    std::uint8_t R, G, B, A;

public:
    Color() noexcept = default;

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

    [[nodiscard]] static Color
    Lerp(const Color& source1, const Color& source2, float amount);

    [[nodiscard]] static Color
    SmoothStep(const Color& source1, const Color& source2, float amount);

    [[nodiscard]] static Color
    Multiply(const Color& color, float scale);

    [[nodiscard]] static Color
    Multiply(const Color& source1, const Color& source2);

    ///@note (R, G, B, A) = (255, 255, 255, 255)
    static const Color White;

    ///@note (R, G, B, A) = (0, 0, 0, 255)
    static const Color Black;

    ///@note (R, G, B, A) = (255, 0, 0, 255)
    static const Color Red;

    ///@note (R, G, B, A) = (0, 255, 0, 255)
    static const Color Green;

    ///@note (R, G, B, A) = (0, 0, 255, 255)
    static const Color Blue;

    ///@note (R, G, B, A) = (255, 255, 0, 255)
    static const Color Yellow;

    ///@note (R, G, B, A) = (100, 149 ,237, 255)
    static const Color CornflowerBlue;

    ///@note (R, G, B, A) = (0, 0, 0, 0)
    static const Color TransparentBlack;
};

} // namespace Pomdog
