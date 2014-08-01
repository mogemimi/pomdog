//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Math/Color.hpp>
#include <Pomdog/Math/Vector3.hpp>
#include <Pomdog/Math/Vector4.hpp>
#include <Pomdog/Math/MathHelper.hpp>
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {

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
: PackedValue(std::uint32_t(red) | (std::uint32_t(green) << 8) | (std::uint32_t(blue) << 16) | (std::uint32_t(alpha) << 24))
{}
//-----------------------------------------------------------------------
Color::Color(Vector3 const& vector)
: Color(static_cast<std::uint8_t>(vector.X*255.0f)
, static_cast<std::uint8_t>(vector.Y*255.0f)
, static_cast<std::uint8_t>(vector.Z*255.0f)
, 255)
{
	POMDOG_ASSERT(vector.X >= 0.0f && vector.X <= 1.0f);
	POMDOG_ASSERT(vector.Y >= 0.0f && vector.Y <= 1.0f);
	POMDOG_ASSERT(vector.Z >= 0.0f && vector.Z <= 1.0f);
}
//-----------------------------------------------------------------------
Color::Color(Vector4 const& vector)
: Color(static_cast<std::uint8_t>(vector.X*255.0f)
, static_cast<std::uint8_t>(vector.Y*255.0f)
, static_cast<std::uint8_t>(vector.Z*255.0f)
, static_cast<std::uint8_t>(vector.W*255.0f))
{
	POMDOG_ASSERT(vector.X >= 0.0f && vector.X <= 1.0f);
	POMDOG_ASSERT(vector.Y >= 0.0f && vector.Y <= 1.0f);
	POMDOG_ASSERT(vector.Z >= 0.0f && vector.Z <= 1.0f);
	POMDOG_ASSERT(vector.W >= 0.0f && vector.W <= 1.0f);
}
//-----------------------------------------------------------------------
Color & Color::operator*=(float scale)
{
	POMDOG_ASSERT(scale >= 0.0f);
	this->R(R() * scale);
	this->G(G() * scale);
	this->B(B() * scale);
	this->A(A() * scale);
	return *this;
}
//-----------------------------------------------------------------------
Color Color::operator*(float scale) const
{
	POMDOG_ASSERT(scale >= 0.0f);
	return Color{
		static_cast<std::uint8_t>(R() * scale),
		static_cast<std::uint8_t>(G() * scale),
		static_cast<std::uint8_t>(B() * scale),
		static_cast<std::uint8_t>(A() * scale)};
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
	return Color(
		source1.R() + amount * (source2.R() - source1.R()),
		source1.G() + amount * (source2.G() - source1.G()),
		source1.B() + amount * (source2.B() - source1.B()),
		source1.A() + amount * (source2.A() - source1.A()));
}
//-----------------------------------------------------------------------
Color Color::SmoothStep(Color const& source1, Color const& source2, float amount)
{
	return Color(
		MathHelper::SmoothStep<float>(source1.R(), source2.R(), amount),
		MathHelper::SmoothStep<float>(source1.G(), source2.G(), amount),
		MathHelper::SmoothStep<float>(source1.B(), source2.B(), amount),
		MathHelper::SmoothStep<float>(source1.A(), source2.A(), amount));
}
//-----------------------------------------------------------------------
Color operator*(float scale, Color const& color)
{
	POMDOG_ASSERT(scale >= 0.0f);
	return Color{
		static_cast<std::uint8_t>(color.R() * scale),
		static_cast<std::uint8_t>(color.G() * scale),
		static_cast<std::uint8_t>(color.B() * scale),
		static_cast<std::uint8_t>(color.A() * scale)};
}

}// namespace Pomdog
