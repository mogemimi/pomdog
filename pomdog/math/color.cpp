// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/color.hpp"
#include "pomdog/basic/compilation_targets.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/math.hpp"
#include "pomdog/math/vector3.hpp"
#include "pomdog/math/vector4.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

std::uint8_t PackUint8(float value)
{
    POMDOG_ASSERT(value <= 255.0f);
    POMDOG_ASSERT(value >= 0);
    POMDOG_ASSERT(!std::isnan(value));
    POMDOG_ASSERT(!std::isinf(value));
    return static_cast<std::uint8_t>(std::round(value));
}

std::uint8_t PackFromNormal(float value)
{
    POMDOG_ASSERT(value <= 1.0f);
    POMDOG_ASSERT(value >= 0);
    POMDOG_ASSERT(!std::isnan(value));
    POMDOG_ASSERT(!std::isinf(value));
    constexpr float scale = 255.0f;
    return PackUint8(value * scale);
}

std::uint32_t ColorPackUint(
    std::uint32_t red, std::uint32_t green,
    std::uint32_t blue, std::uint32_t alpha) noexcept
{
    return (((red | (green << 8)) | (blue << 16)) | (alpha << 24));
}

} // namespace

Color::Color() noexcept = default;

Color::Color(
    std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) noexcept
    : R(r)
    , G(g)
    , B(b)
    , A(a)
{
}

Color::Color(const Vector3& vector)
    : R(PackFromNormal(vector.X))
    , G(PackFromNormal(vector.Y))
    , B(PackFromNormal(vector.Z))
    , A(255)
{
    POMDOG_ASSERT(vector.X >= 0.0f && vector.X <= 1.0f);
    POMDOG_ASSERT(vector.Y >= 0.0f && vector.Y <= 1.0f);
    POMDOG_ASSERT(vector.Z >= 0.0f && vector.Z <= 1.0f);
}

Color::Color(const Vector4& vector)
    : R(PackFromNormal(vector.X))
    , G(PackFromNormal(vector.Y))
    , B(PackFromNormal(vector.Z))
    , A(PackFromNormal(vector.W))
{
    POMDOG_ASSERT(vector.X >= 0.0f && vector.X <= 1.0f);
    POMDOG_ASSERT(vector.Y >= 0.0f && vector.Y <= 1.0f);
    POMDOG_ASSERT(vector.Z >= 0.0f && vector.Z <= 1.0f);
    POMDOG_ASSERT(vector.W >= 0.0f && vector.W <= 1.0f);
}

bool Color::operator==(const Color& color) const noexcept
{
    return R == color.R && G == color.G && B == color.B && A == color.A;
}

bool Color::operator!=(const Color& color) const noexcept
{
    return R == color.R || G == color.G || B == color.B || A == color.A;
}

Vector3 Color::ToVector3() const noexcept
{
    return {R / 255.0f, G / 255.0f, B / 255.0f};
}

Vector4 Color::ToVector4() const noexcept
{
    return {R / 255.0f, G / 255.0f, B / 255.0f, A / 255.0f};
}

std::uint32_t Color::ToPackedValue() const noexcept
{
    return ColorPackUint(R, G, B, A);
}

Color Color::FromPackedValue(std::uint32_t packedValue)
{
    Color color;
#if defined(POMDOG_BYTEORDER_BIG_ENDIAN)
    color.R = static_cast<std::uint8_t>(packedValue);
    color.G = static_cast<std::uint8_t>(packedValue >> 8);
    color.B = static_cast<std::uint8_t>(packedValue >> 16);
    color.A = static_cast<std::uint8_t>(packedValue >> 24);
#else
    color.R = static_cast<std::uint8_t>(packedValue >> 24);
    color.G = static_cast<std::uint8_t>(packedValue >> 16);
    color.B = static_cast<std::uint8_t>(packedValue >> 8);
    color.A = static_cast<std::uint8_t>(packedValue);
#endif
    return color;
}

Color Color::White() noexcept
{
    return Color{255, 255, 255, 255};
}

Color Color::Black() noexcept
{
    return Color{0, 0, 0, 255};
}

Color Color::Red() noexcept
{
    return Color{255, 0, 0, 255};
}

Color Color::Green() noexcept
{
    return Color{0, 255, 0, 255};
}

Color Color::Blue() noexcept
{
    return Color{0, 0, 255, 255};
}

Color Color::Yellow() noexcept
{
    return Color{255, 255, 0, 255};
}

Color Color::CornflowerBlue() noexcept
{
    return Color{100, 149, 237, 255};
}

Color Color::TransparentBlack() noexcept
{
    return Color{0, 0, 0, 0};
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] Color
Multiply(const Color& color, float factor)
{
    Color result;
    result.R = PackUint8(math::Clamp(color.R * factor, 0.0f, 255.0f));
    result.G = PackUint8(math::Clamp(color.G * factor, 0.0f, 255.0f));
    result.B = PackUint8(math::Clamp(color.B * factor, 0.0f, 255.0f));
    result.A = PackUint8(math::Clamp(color.A * factor, 0.0f, 255.0f));
    return result;
}

[[nodiscard]] Color
Multiply(const Color& color1, const Color& color2)
{
    Color result;
    result.R = PackUint8(math::Clamp((static_cast<float>(color1.R) / 255.0f) * static_cast<float>(color2.R), 0.0f, 255.0f));
    result.G = PackUint8(math::Clamp((static_cast<float>(color1.G) / 255.0f) * static_cast<float>(color2.G), 0.0f, 255.0f));
    result.B = PackUint8(math::Clamp((static_cast<float>(color1.B) / 255.0f) * static_cast<float>(color2.B), 0.0f, 255.0f));
    result.A = PackUint8(math::Clamp((static_cast<float>(color1.A) / 255.0f) * static_cast<float>(color2.A), 0.0f, 255.0f));
    return result;
}

[[nodiscard]] Color
Lerp(const Color& source1, const Color& source2, float amount)
{
    Color color;
    color.R = PackUint8(source1.R + amount * (source2.R - source1.R));
    color.G = PackUint8(source1.G + amount * (source2.G - source1.G));
    color.B = PackUint8(source1.B + amount * (source2.B - source1.B));
    color.A = PackUint8(source1.A + amount * (source2.A - source1.A));
    return color;
}

[[nodiscard]] Color
SmoothStep(const Color& source1, const Color& source2, float amount)
{
    Color color;
    color.R = PackUint8(math::SmoothStep(static_cast<float>(source1.R), static_cast<float>(source2.R), amount));
    color.G = PackUint8(math::SmoothStep(static_cast<float>(source1.G), static_cast<float>(source2.G), amount));
    color.B = PackUint8(math::SmoothStep(static_cast<float>(source1.B), static_cast<float>(source2.B), amount));
    color.A = PackUint8(math::SmoothStep(static_cast<float>(source1.A), static_cast<float>(source2.A), amount));
    return color;
}

} // namespace pomdog::math
