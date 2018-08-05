// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/Color.hpp"
#include "../Basic/ConditionalCompilation.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cmath>
#include <utility>

namespace Pomdog {
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

} // unnamed namespace

const Color Color::White{255, 255, 255, 255};
const Color Color::Black{0, 0, 0, 255};
const Color Color::Red{255, 0, 0, 255};
const Color Color::Green{0, 255, 0, 255};
const Color Color::Blue{0, 0, 255, 255};
const Color Color::Yellow{255, 255, 0, 255};
const Color Color::CornflowerBlue{100, 149, 237, 255};
const Color Color::TransparentBlack{0, 0, 0, 0};

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

Color Color::Lerp(const Color& source1, const Color& source2, float amount)
{
    Color color;
    color.R = PackUint8(source1.R + amount * (source2.R - source1.R));
    color.G = PackUint8(source1.G + amount * (source2.G - source1.G));
    color.B = PackUint8(source1.B + amount * (source2.B - source1.B));
    color.A = PackUint8(source1.A + amount * (source2.A - source1.A));
    return color;
}

Color Color::SmoothStep(const Color& source1, const Color& source2, float amount)
{
    Color color;
    color.R = PackUint8(MathHelper::SmoothStep<float>(source1.R, source2.R, amount));
    color.G = PackUint8(MathHelper::SmoothStep<float>(source1.G, source2.G, amount));
    color.B = PackUint8(MathHelper::SmoothStep<float>(source1.B, source2.B, amount));
    color.A = PackUint8(MathHelper::SmoothStep<float>(source1.A, source2.A, amount));
    return color;
}

Color Color::Multiply(const Color& color, float scale)
{
    Color result;
    result.R = PackUint8(MathHelper::Clamp(color.R * scale, 0.0f, 255.0f));
    result.G = PackUint8(MathHelper::Clamp(color.G * scale, 0.0f, 255.0f));
    result.B = PackUint8(MathHelper::Clamp(color.B * scale, 0.0f, 255.0f));
    result.A = PackUint8(MathHelper::Clamp(color.A * scale, 0.0f, 255.0f));
    return result;
}

Color Color::Multiply(const Color& color1, const Color& color2)
{
    Color result;
    result.R = PackUint8(MathHelper::Clamp((color1.R / 255.0f) * color2.R, 0.0f, 255.0f));
    result.G = PackUint8(MathHelper::Clamp((color1.G / 255.0f) * color2.G, 0.0f, 255.0f));
    result.B = PackUint8(MathHelper::Clamp((color1.B / 255.0f) * color2.B, 0.0f, 255.0f));
    result.A = PackUint8(MathHelper::Clamp((color1.A / 255.0f) * color2.A, 0.0f, 255.0f));
    return result;
}

} // namespace Pomdog
