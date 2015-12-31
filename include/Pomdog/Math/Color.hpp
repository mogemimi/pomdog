// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>

namespace Pomdog {

class POMDOG_EXPORT Color {
public:
    std::uint8_t R, G, B, A;

public:
    Color() noexcept = default;

    ///@brief Construct from unsigned integer values.
    Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) noexcept;

    explicit Color(Vector3 const& vector);
    explicit Color(Vector4 const& vector);

    bool operator==(Color const&) const noexcept;
    bool operator!=(Color const&) const noexcept;

    Vector3 ToVector3() const noexcept;
    Vector4 ToVector4() const noexcept;

    std::uint32_t ToPackedValue() const noexcept;

    static Color FromPackedValue(std::uint32_t packedValue);

    static Color Lerp(Color const& source1, Color const& source2, float amount);

    static Color SmoothStep(Color const& source1, Color const& source2, float amount);

    static Color Multiply(Color const& color, float scale);

    static Color Multiply(Color const& source1, Color const& source2);

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
