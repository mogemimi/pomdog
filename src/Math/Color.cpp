// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cmath>
#include <utility>

namespace Pomdog {
namespace {

static std::uint32_t PackUnsignedNormal(float value)
{
    POMDOG_ASSERT(value <= 255.0f);
    POMDOG_ASSERT(value >= 0);
    POMDOG_ASSERT(!std::isnan(value));
    POMDOG_ASSERT(!std::isinf(value));
    return static_cast<std::uint32_t>(value);
}

static std::uint32_t ColorPackUint(std::uint32_t red, std::uint32_t green, std::uint32_t blue, std::uint32_t alpha)
{
    return (((red | (green << 8)) | (blue << 16)) | (alpha << 24));
}

static std::uint32_t ColorPackFromNormal(float red, float green, float blue, float alpha)
{
    constexpr float scale = 255.0f;
    const auto r = PackUnsignedNormal(scale * red);
    const auto g = PackUnsignedNormal(scale * green);
    const auto b = PackUnsignedNormal(scale * blue);
    const auto a = PackUnsignedNormal(scale * alpha);
    return ColorPackUint(r, g, b, a);
}

static std::uint32_t ColorPackFromFloat(float red, float green, float blue, float alpha)
{
    const auto r = PackUnsignedNormal(red);
    const auto g = PackUnsignedNormal(green);
    const auto b = PackUnsignedNormal(blue);
    const auto a = PackUnsignedNormal(alpha);
    return ColorPackUint(r, g, b, a);
}

} // unnamed namespace

const Color Color::White{255, 255, 255, 255};
const Color Color::Black{0, 0, 0, 255};
const Color Color::Red{255, 0, 0, 255};
const Color Color::Green{0, 255, 0, 255};
const Color Color::Blue{0, 0, 255, 255};
const Color Color::Yellow{255, 255, 0, 255};
const Color Color::CornflowerBlue{100, 149 ,237, 255};
const Color Color::TransparentBlack{0, 0, 0, 0};
//-----------------------------------------------------------------------
Color::Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha)
    : PackedValue{ ColorPackUint(red, green, blue, alpha) }
{}
//-----------------------------------------------------------------------
Color::Color(Vector3 const& vector)
    : PackedValue{ ColorPackFromNormal(vector.X, vector.Y, vector.Z, 1.0f) }
{
    POMDOG_ASSERT(vector.X >= 0.0f && vector.X <= 1.0f);
    POMDOG_ASSERT(vector.Y >= 0.0f && vector.Y <= 1.0f);
    POMDOG_ASSERT(vector.Z >= 0.0f && vector.Z <= 1.0f);
}
//-----------------------------------------------------------------------
Color::Color(Vector4 const& vector)
    : PackedValue{ ColorPackFromNormal(vector.X, vector.Y, vector.Z, vector.W) }
{
    POMDOG_ASSERT(vector.X >= 0.0f && vector.X <= 1.0f);
    POMDOG_ASSERT(vector.Y >= 0.0f && vector.Y <= 1.0f);
    POMDOG_ASSERT(vector.Z >= 0.0f && vector.Z <= 1.0f);
    POMDOG_ASSERT(vector.W >= 0.0f && vector.W <= 1.0f);
}
//-----------------------------------------------------------------------
bool Color::operator==(Color const& color) const
{
    return this->PackedValue == color.PackedValue;
}
//-----------------------------------------------------------------------
bool Color::operator!=(Color const& color) const
{
    return this->PackedValue != color.PackedValue;
}
//-----------------------------------------------------------------------
std::uint8_t Color::R() const
{
    return static_cast<std::uint8_t>(PackedValue);
}
//-----------------------------------------------------------------------
std::uint8_t Color::G() const
{
    return static_cast<std::uint8_t>(PackedValue >> 8);
}
//-----------------------------------------------------------------------
std::uint8_t Color::B() const
{
    return static_cast<std::uint8_t>(PackedValue >> 16);
}
//-----------------------------------------------------------------------
std::uint8_t Color::A() const
{
    return static_cast<std::uint8_t>(PackedValue >> 24);
}
//-----------------------------------------------------------------------
void Color::R(std::uint8_t value)
{
    PackedValue = ((PackedValue & 0xffffff00) | value);
}
//-----------------------------------------------------------------------
void Color::G(std::uint8_t value)
{
    PackedValue = ((PackedValue & 0xffff00ff) | (static_cast<std::uint32_t>(value) << 8));
}
//-----------------------------------------------------------------------
void Color::B(std::uint8_t value)
{
    PackedValue = ((PackedValue & 0xff00ffff) | (static_cast<std::uint32_t>(value) << 16));
}
//-----------------------------------------------------------------------
void Color::A(std::uint8_t value)
{
    PackedValue = ((PackedValue & 0x00ffffff) | (static_cast<std::uint32_t>(value) << 24));
}
//-----------------------------------------------------------------------
Vector3 Color::ToVector3() const
{
    return {R()/255.0f, G()/255.0f, B()/255.0f};
}
//-----------------------------------------------------------------------
Vector4 Color::ToVector4() const
{
    return {R()/255.0f, G()/255.0f, B()/255.0f, A()/255.0f};
}
//-----------------------------------------------------------------------
Color Color::Lerp(Color const& source1, Color const& source2, float amount)
{
    Color color;
    color.PackedValue = ColorPackFromFloat(
        source1.R() + amount * (source2.R() - source1.R()),
        source1.G() + amount * (source2.G() - source1.G()),
        source1.B() + amount * (source2.B() - source1.B()),
        source1.A() + amount * (source2.A() - source1.A()));
    return std::move(color);
}
//-----------------------------------------------------------------------
Color Color::SmoothStep(Color const& source1, Color const& source2, float amount)
{
    Color color;
    color.PackedValue = ColorPackFromFloat(
        MathHelper::SmoothStep<float>(source1.R(), source2.R(), amount),
        MathHelper::SmoothStep<float>(source1.G(), source2.G(), amount),
        MathHelper::SmoothStep<float>(source1.B(), source2.B(), amount),
        MathHelper::SmoothStep<float>(source1.A(), source2.A(), amount));
    return std::move(color);
}

} // namespace Pomdog
