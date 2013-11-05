//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <pomdog/Math/Color.hpp>
#include <pomdog/Utility/Assert.hpp>
#include <type_traits>

namespace Pomdog {

Color Color::White				(1.0f, 1.0f, 1.0f, 1.0f);
Color Color::Black				(0.0f, 0.0f, 0.0f, 1.0f);
Color Color::Red				(1.0f, 0.0f, 0.0f, 1.0f);
Color Color::Green				(0.0f, 1.0f, 0.0f, 1.0f);
Color Color::Blue				(0.0f, 0.0f, 1.0f, 1.0f);
Color Color::Yellow				(1.0f, 1.0f, 0.0f, 1.0f);
Color Color::CornflowerBlue		(100.0f/255.0f, 149.0f/255.0f, 237.0f/255.0f, 1.0f);
Color Color::TransparentBlack	(0.0f, 0.0f, 0.0f, 0.0f);
//-----------------------------------------------------------------------
Color::Color(float red, float green, float blue, float alpha)
	: r(red), g(green), b(blue), a(alpha)
{}
//-----------------------------------------------------------------------
Color & Color::operator += (Color const& right)
{
	r += right.r;
	g += right.g;
	b += right.b;
	a += right.a;
	return *this;
}
//-----------------------------------------------------------------------
Color & Color::operator -= (Color const& right)
{
	r -= right.r;
	g -= right.g;
	b -= right.b;
	a -= right.a;
	return *this;
}
//-----------------------------------------------------------------------
Color & Color::operator *= (float scaleFactor)
{
	r *= scaleFactor;
	g *= scaleFactor;
	b *= scaleFactor;
	a *= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
Color & Color::operator /= (float scaleFactor)
{
	r /= scaleFactor;
	g /= scaleFactor;
	b /= scaleFactor;
	a /= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
Color Color::operator + () const
{
	return *this;
}
//-----------------------------------------------------------------------
Color Color::operator - () const
{
	return Color(-r, -g, -b, -a);
}
//-----------------------------------------------------------------------
Color Color::operator + (Color const& right) const
{
	return Color(r + right.r, g + right.g, b + right.b, a + right.a);
}
//-----------------------------------------------------------------------
Color Color::operator - (Color const& right) const
{
	return Color(r - right.r, g - right.g, b - right.b, a - right.a);
}
//-----------------------------------------------------------------------
Color Color::operator * (float scaleFactor) const
{
	return Color(r * scaleFactor, g * scaleFactor, b * scaleFactor, a * scaleFactor);
}
//-----------------------------------------------------------------------
Color Color::operator / (float scaleFactor) const
{
	return Color(r / scaleFactor, g / scaleFactor, b / scaleFactor, a / scaleFactor);
}
//-----------------------------------------------------------------------
Color operator * (float left, Color const& right)
{
	return right * left;
}
//-----------------------------------------------------------------------
bool Color::operator == (Color const& right) const
{
	return (r == right.r && g == right.g && b == right.b && a == right.a);
}
//-----------------------------------------------------------------------
bool Color::operator != (Color const& right) const
{
	return (r != right.r || g != right.g || b != right.b || a != right.a);
}
//-----------------------------------------------------------------------
float & Color::operator [] (size_t index)
{
	POMDOG_ASSERT_MESSAGE(index < 4, "index: out of range");
	return *(reinterpret_cast<float*>(&this->r) + index);
}
//-----------------------------------------------------------------------
float Color::operator [] (size_t index) const
{
	POMDOG_ASSERT_MESSAGE(index < 4, "index: out of range");
	return *(reinterpret_cast<float const*>(&this->r) + index);
}
//-----------------------------------------------------------------------
void Color::Set(float red, float green, float blue, float alpha)
{
	this->r = red;
	this->g = green;
	this->b = blue;
	this->a = alpha;
}
//-----------------------------------------------------------------------
template <typename T>
static T SaturateImpl(T x)
{
	static_assert(std::is_floating_point<T>::value, "");
	if (x < static_cast<T>(0))
		return static_cast<T>(0);
	else if (x > static_cast<T>(1))
		return static_cast<T>(1);
	return x;
}
//-------------------------------------------------------------------
void Color::Saturate()
{
	r = SaturateImpl(r);
	g = SaturateImpl(g);
	b = SaturateImpl(b);
	a = SaturateImpl(a);
}
//-----------------------------------------------------------------------
float const* Color::Data() const
{
	return &r;
}
//-----------------------------------------------------------------------
float* Color::Data()
{
	return &r;
}
//-----------------------------------------------------------------------
Color Color::CreateHexRGBA(std::uint32_t rgba)
{
	Color color;
#if defined(POMDOG_BYTEORDER_LITTLE_ENDIAN)
	color.r = static_cast<float>((rgba >> 24) & 0xff) / 255.0f;	// 0x1234ABCD >> 24 = 0x12
	color.g = static_cast<float>((rgba >> 16) & 0xff) / 255.0f;	// 0x1234ABCD >> 16 = 0x1234
	color.b = static_cast<float>((rgba >> 8) & 0xff) / 255.0f;	// 0x1234ABCD >>  8 = 0x1234AB
	color.a = static_cast<float>(rgba & 0xff) / 255.0f;			// 0x1234ABCD >>  0 = 0x1234ABCD
#elif defined(POMDOG_BYTEORDER_BIG_ENDIAN)
	color.a = static_cast<float>((rgba >> 24) & 0xff) / 255.0f;
	color.b = static_cast<float>((rgba >> 16) & 0xff) / 255.0f;
	color.g = static_cast<float>((rgba >> 8) & 0xff) / 255.0f;
	color.r = static_cast<float>(rgba & 0xff) / 255.0f;
#else
#	error "Byte order undefined or not supported."
#endif
	return color;
}
//-----------------------------------------------------------------------
Color Color::CreateHexARGB(std::uint32_t argb)
{
	Color color;
#if defined(POMDOG_BYTEORDER_LITTLE_ENDIAN)
	color.a = static_cast<float>((argb >> 24) & 0xff) / 255.0f;	// 0x1234ABCD >> 24 = 0x12
	color.r = static_cast<float>((argb >> 16) & 0xff) / 255.0f;	// 0x1234ABCD >> 16 = 0x1234
	color.g = static_cast<float>((argb >> 8) & 0xff) / 255.0f;	// 0x1234ABCD >>  8 = 0x1234AB
	color.b = static_cast<float>(argb & 0xff) / 255.0f;			// 0x1234ABCD >>  0 = 0x1234ABCD
#elif defined(POMDOG_BYTEORDER_BIG_ENDIAN)
	color.b = static_cast<float>((argb >> 24) & 0xff) / 255.0f;
	color.g = static_cast<float>((argb >> 16) & 0xff) / 255.0f;
	color.r = static_cast<float>((argb >> 8) & 0xff) / 255.0f;
	color.a = static_cast<float>(argb & 0xff) / 255.0f;
#else
#	error "Byte order undefined or not supported."
#endif
	return color;
}
//-----------------------------------------------------------------------
Color Color::CreateLerp(Color const& source0, Color const& source1, float amount)
{
	return Color( 
		source0.r + amount * (source1.r - source0.r), 
		source0.g + amount * (source1.g - source0.g),
		source0.b + amount * (source1.b - source0.b), 
		source0.a + amount * (source1.a - source0.a));
}

}// namespace Pomdog
