// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/color.h"
#include "pomdog/basic/compilation_targets.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

[[nodiscard]] std::uint8_t PackUint8(float value) noexcept
{
    POMDOG_ASSERT(value <= 255.0f);
    POMDOG_ASSERT(value >= 0);
    POMDOG_ASSERT(!std::isnan(value));
    POMDOG_ASSERT(!std::isinf(value));
    return static_cast<std::uint8_t>(std::round(value));
}

[[nodiscard]] std::uint8_t PackFromNormal(float value) noexcept
{
    POMDOG_ASSERT(value <= 1.0f);
    POMDOG_ASSERT(value >= 0);
    POMDOG_ASSERT(!std::isnan(value));
    POMDOG_ASSERT(!std::isinf(value));
    constexpr float scale = 255.0f;
    return PackUint8(value * scale);
}

[[nodiscard]] std::uint32_t ColorPackUint(
    std::uint32_t red,
    std::uint32_t green,
    std::uint32_t blue,
    std::uint32_t alpha) noexcept
{
    return (((red | (green << 8)) | (blue << 16)) | (alpha << 24));
}

} // namespace

Color::Color() noexcept = default;

Color::Color(std::uint8_t rIn, std::uint8_t gIn, std::uint8_t bIn, std::uint8_t aIn) noexcept
    : r(rIn)
    , g(gIn)
    , b(bIn)
    , a(aIn)
{
}

Color::Color(const Vector3& vector)
    : r(PackFromNormal(vector.x))
    , g(PackFromNormal(vector.y))
    , b(PackFromNormal(vector.z))
    , a(255)
{
    POMDOG_ASSERT(vector.x >= 0.0f && vector.x <= 1.0f);
    POMDOG_ASSERT(vector.y >= 0.0f && vector.y <= 1.0f);
    POMDOG_ASSERT(vector.z >= 0.0f && vector.z <= 1.0f);
}

Color::Color(const Vector4& vector)
    : r(PackFromNormal(vector.x))
    , g(PackFromNormal(vector.y))
    , b(PackFromNormal(vector.z))
    , a(PackFromNormal(vector.w))
{
    POMDOG_ASSERT(vector.x >= 0.0f && vector.x <= 1.0f);
    POMDOG_ASSERT(vector.y >= 0.0f && vector.y <= 1.0f);
    POMDOG_ASSERT(vector.z >= 0.0f && vector.z <= 1.0f);
    POMDOG_ASSERT(vector.w >= 0.0f && vector.w <= 1.0f);
}

bool Color::operator==(const Color& color) const noexcept
{
    return r == color.r && g == color.g && b == color.b && a == color.a;
}

bool Color::operator!=(const Color& color) const noexcept
{
    return r != color.r || g != color.g || b != color.b || a != color.a;
}

Vector3 Color::ToVector3() const noexcept
{
    return {r / 255.0f, g / 255.0f, b / 255.0f};
}

Vector4 Color::ToVector4() const noexcept
{
    return {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
}

std::uint32_t Color::ToPackedValue() const noexcept
{
    return ColorPackUint(r, g, b, a);
}

Color Color::FromPackedValue(std::uint32_t packedValue)
{
    Color color;
#if defined(POMDOG_BYTEORDER_BIG_ENDIAN)
    color.r = static_cast<std::uint8_t>(packedValue);
    color.g = static_cast<std::uint8_t>(packedValue >> 8);
    color.b = static_cast<std::uint8_t>(packedValue >> 16);
    color.a = static_cast<std::uint8_t>(packedValue >> 24);
#else
    color.r = static_cast<std::uint8_t>(packedValue >> 24);
    color.g = static_cast<std::uint8_t>(packedValue >> 16);
    color.b = static_cast<std::uint8_t>(packedValue >> 8);
    color.a = static_cast<std::uint8_t>(packedValue);
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
    result.r = PackUint8(math::Clamp(color.r * factor, 0.0f, 255.0f));
    result.g = PackUint8(math::Clamp(color.g * factor, 0.0f, 255.0f));
    result.b = PackUint8(math::Clamp(color.b * factor, 0.0f, 255.0f));
    result.a = PackUint8(math::Clamp(color.a * factor, 0.0f, 255.0f));
    return result;
}

[[nodiscard]] Color
Multiply(const Color& color1, const Color& color2)
{
    Color result;
    result.r = PackUint8(math::Clamp((static_cast<float>(color1.r) / 255.0f) * static_cast<float>(color2.r), 0.0f, 255.0f));
    result.g = PackUint8(math::Clamp((static_cast<float>(color1.g) / 255.0f) * static_cast<float>(color2.g), 0.0f, 255.0f));
    result.b = PackUint8(math::Clamp((static_cast<float>(color1.b) / 255.0f) * static_cast<float>(color2.b), 0.0f, 255.0f));
    result.a = PackUint8(math::Clamp((static_cast<float>(color1.a) / 255.0f) * static_cast<float>(color2.a), 0.0f, 255.0f));
    return result;
}

[[nodiscard]] Color
Lerp(const Color& source1, const Color& source2, float amount)
{
    Color color;
    color.r = PackUint8(source1.r + amount * (source2.r - source1.r));
    color.g = PackUint8(source1.g + amount * (source2.g - source1.g));
    color.b = PackUint8(source1.b + amount * (source2.b - source1.b));
    color.a = PackUint8(source1.a + amount * (source2.a - source1.a));
    return color;
}

[[nodiscard]] Color
SmoothStep(const Color& source1, const Color& source2, float amount)
{
    Color color;
    color.r = PackUint8(math::SmoothStep(static_cast<float>(source1.r), static_cast<float>(source2.r), amount));
    color.g = PackUint8(math::SmoothStep(static_cast<float>(source1.g), static_cast<float>(source2.g), amount));
    color.b = PackUint8(math::SmoothStep(static_cast<float>(source1.b), static_cast<float>(source2.b), amount));
    color.a = PackUint8(math::SmoothStep(static_cast<float>(source1.a), static_cast<float>(source2.a), amount));
    return color;
}

} // namespace pomdog::math
