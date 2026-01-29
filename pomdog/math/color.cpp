// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/color.h"
#include "pomdog/basic/compilation_target_architecture.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

[[nodiscard]] u8 packUint8(f32 value) noexcept
{
    POMDOG_ASSERT(value <= 255.0f);
    POMDOG_ASSERT(value >= 0);
    POMDOG_ASSERT(!std::isnan(value));
    POMDOG_ASSERT(!std::isinf(value));
    return static_cast<u8>(std::round(value));
}

[[nodiscard]] u8 packFromNormal(f32 value) noexcept
{
    POMDOG_ASSERT(value <= 1.0f);
    POMDOG_ASSERT(value >= 0);
    POMDOG_ASSERT(!std::isnan(value));
    POMDOG_ASSERT(!std::isinf(value));
    constexpr f32 scale = 255.0f;
    return packUint8(value * scale);
}

[[nodiscard]] u32 colorPackUint(u32 r, u32 g, u32 b, u32 a) noexcept
{
    return (((r | (g << 8)) | (b << 16)) | (a << 24));
}

} // namespace

Color::Color() noexcept = default;

Color::Color(u8 rIn, u8 gIn, u8 bIn, u8 aIn) noexcept
    : r(rIn)
    , g(gIn)
    , b(bIn)
    , a(aIn)
{
}

Color::Color(const Vector3& vector)
    : r(packFromNormal(vector.x))
    , g(packFromNormal(vector.y))
    , b(packFromNormal(vector.z))
    , a(255)
{
    POMDOG_ASSERT(vector.x >= 0.0f && vector.x <= 1.0f);
    POMDOG_ASSERT(vector.y >= 0.0f && vector.y <= 1.0f);
    POMDOG_ASSERT(vector.z >= 0.0f && vector.z <= 1.0f);
}

Color::Color(const Vector4& vector)
    : r(packFromNormal(vector.x))
    , g(packFromNormal(vector.y))
    , b(packFromNormal(vector.z))
    , a(packFromNormal(vector.w))
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

Vector3
Color::toVector3() const noexcept
{
    return {r / 255.0f, g / 255.0f, b / 255.0f};
}

Vector4
Color::toVector4() const noexcept
{
    return {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
}

u32 Color::toPackedValue() const noexcept
{
    return colorPackUint(r, g, b, a);
}

Color Color::fromPackedValue(u32 packedValue) noexcept
{
    Color color;
#if defined(POMDOG_BYTEORDER_BIG_ENDIAN)
    color.r = static_cast<u8>(packedValue);
    color.g = static_cast<u8>(packedValue >> 8);
    color.b = static_cast<u8>(packedValue >> 16);
    color.a = static_cast<u8>(packedValue >> 24);
#else
    color.r = static_cast<u8>(packedValue >> 24);
    color.g = static_cast<u8>(packedValue >> 16);
    color.b = static_cast<u8>(packedValue >> 8);
    color.a = static_cast<u8>(packedValue);
#endif
    return color;
}

Color Color::createWhite() noexcept
{
    return Color{255, 255, 255, 255};
}

Color Color::createBlack() noexcept
{
    return Color{0, 0, 0, 255};
}

Color Color::createRed() noexcept
{
    return Color{255, 0, 0, 255};
}

Color Color::createGreen() noexcept
{
    return Color{0, 255, 0, 255};
}

Color Color::createBlue() noexcept
{
    return Color{0, 0, 255, 255};
}

Color Color::createYellow() noexcept
{
    return Color{255, 255, 0, 255};
}

Color Color::createCornflowerBlue() noexcept
{
    return Color{100, 149, 237, 255};
}

Color Color::createTransparentBlack() noexcept
{
    return Color{0, 0, 0, 0};
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] Color
multiply(const Color& color, f32 factor)
{
    Color result;
    result.r = packUint8(math::clamp(color.r * factor, 0.0f, 255.0f));
    result.g = packUint8(math::clamp(color.g * factor, 0.0f, 255.0f));
    result.b = packUint8(math::clamp(color.b * factor, 0.0f, 255.0f));
    result.a = packUint8(math::clamp(color.a * factor, 0.0f, 255.0f));
    return result;
}

[[nodiscard]] Color
multiply(const Color& color1, const Color& color2)
{
    Color result;
    result.r = packUint8(math::clamp((static_cast<f32>(color1.r) / 255.0f) * static_cast<f32>(color2.r), 0.0f, 255.0f));
    result.g = packUint8(math::clamp((static_cast<f32>(color1.g) / 255.0f) * static_cast<f32>(color2.g), 0.0f, 255.0f));
    result.b = packUint8(math::clamp((static_cast<f32>(color1.b) / 255.0f) * static_cast<f32>(color2.b), 0.0f, 255.0f));
    result.a = packUint8(math::clamp((static_cast<f32>(color1.a) / 255.0f) * static_cast<f32>(color2.a), 0.0f, 255.0f));
    return result;
}

[[nodiscard]] Color
lerp(const Color& source1, const Color& source2, float amount)
{
    Color color;
    color.r = packUint8(source1.r + amount * (source2.r - source1.r));
    color.g = packUint8(source1.g + amount * (source2.g - source1.g));
    color.b = packUint8(source1.b + amount * (source2.b - source1.b));
    color.a = packUint8(source1.a + amount * (source2.a - source1.a));
    return color;
}

[[nodiscard]] Color
smoothstep(const Color& source1, const Color& source2, f32 amount)
{
    Color color;
    color.r = packUint8(math::smoothstep(static_cast<f32>(source1.r), static_cast<f32>(source2.r), amount));
    color.g = packUint8(math::smoothstep(static_cast<f32>(source1.g), static_cast<f32>(source2.g), amount));
    color.b = packUint8(math::smoothstep(static_cast<f32>(source1.b), static_cast<f32>(source2.b), amount));
    color.a = packUint8(math::smoothstep(static_cast<f32>(source1.a), static_cast<f32>(source2.a), amount));
    return color;
}

} // namespace pomdog::math
