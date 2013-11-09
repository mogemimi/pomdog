//
//  Copyright (C) 2013 the Pomdog Engine authors.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <pomdog/Math/detail/FloatingPointColor.hpp>
#include <pomdog/Utility/Assert.hpp>

namespace Pomdog {
namespace Details {

template <typename T> FloatingPointColor<T> FloatingPointColor<T>::White(1, 1, 1, 1);
template <typename T> FloatingPointColor<T> FloatingPointColor<T>::Black(0, 0, 0, 1);
template <typename T> FloatingPointColor<T> FloatingPointColor<T>::Red(1, 0, 0, 1);
template <typename T> FloatingPointColor<T> FloatingPointColor<T>::Green(0, 1, 0, 1);
template <typename T> FloatingPointColor<T> FloatingPointColor<T>::Blue(0, 0, 1, 1);
template <typename T> FloatingPointColor<T> FloatingPointColor<T>::Yellow(1, 1, 0, 1);
template <typename T> FloatingPointColor<T> FloatingPointColor<T>::CornflowerBlue(100.0/255.0, 149.0/255.0, 237.0/255.0, 1);
template <typename T> FloatingPointColor<T> FloatingPointColor<T>::TransparentBlack(0, 0, 0, 0);

//-----------------------------------------------------------------------
template <typename T>
FloatingPointColor<T>::FloatingPointColor(T red, T green, T blue, T alpha)
	: r(red), g(green), b(blue), a(alpha)
{}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointColor<T> & FloatingPointColor<T>::operator+=(FloatingPointColor const& other)
{
	r += other.r;
	g += other.g;
	b += other.b;
	a += other.a;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointColor<T> & FloatingPointColor<T>::operator-=(FloatingPointColor const& other)
{
	r -= other.r;
	g -= other.g;
	b -= other.b;
	a -= other.a;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointColor<T> & FloatingPointColor<T>::operator*=(T scaleFactor)
{
	r *= scaleFactor;
	g *= scaleFactor;
	b *= scaleFactor;
	a *= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointColor<T> & FloatingPointColor<T>::operator/=(T scaleFactor)
{
	r /= scaleFactor;
	g /= scaleFactor;
	b /= scaleFactor;
	a /= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointColor<T> FloatingPointColor<T>::operator+() const
{
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointColor<T> FloatingPointColor<T>::operator-() const
{
	return FloatingPointColor(-r, -g, -b, -a);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointColor<T> FloatingPointColor<T>::operator+(FloatingPointColor const& other) const
{
	return FloatingPointColor(
		r + other.r,
		g + other.g,
		b + other.b,
		a + other.a);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointColor<T> FloatingPointColor<T>::operator-(FloatingPointColor const& other) const
{
	return FloatingPointColor(
		r - other.r,
		g - other.g,
		b - other.b,
		a - other.a);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointColor<T> FloatingPointColor<T>::operator*(T scaleFactor) const
{
	return FloatingPointColor(
		r * scaleFactor,
		g * scaleFactor,
		b * scaleFactor,
		a * scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointColor<T> FloatingPointColor<T>::operator/(T scaleFactor) const
{
	return FloatingPointColor(
		r / scaleFactor,
		g / scaleFactor,
		b / scaleFactor,
		a / scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointColor<T> operator*(T scaleFactor, FloatingPointColor<T> const& other)
{
	return FloatingPointColor<T>(
		other.r * scaleFactor,
		other.g * scaleFactor,
		other.b * scaleFactor,
		other.a * scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointColor<T>::operator==(FloatingPointColor const& other) const
{
	return (r == other.r && g == other.g && b == other.b && a == other.a);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointColor<T>::operator!=(FloatingPointColor const& other) const
{
	return (r != other.r || g != other.g || b != other.b || a != other.a);
}
//-----------------------------------------------------------------------
template <typename T>
void FloatingPointColor<T>::Set(T red, T green, T blue, T alpha)
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
template <typename T>
void FloatingPointColor<T>::Saturate()
{
	r = SaturateImpl(r);
	g = SaturateImpl(g);
	b = SaturateImpl(b);
	a = SaturateImpl(a);
}
//-----------------------------------------------------------------------
template <typename T>
T const* FloatingPointColor<T>::Data() const
{
	static_assert(std::is_fundamental<T>::value, "T is fundamental type");
	return &r;
}
//-----------------------------------------------------------------------
template <typename T>
T* FloatingPointColor<T>::Data()
{
	static_assert(std::is_fundamental<T>::value, "T is fundamental type");
	return &r;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointColor<T> FloatingPointColor<T>::CreateHexRGBA(std::uint32_t rgba)
{
	constexpr T divisor = static_cast<T>(255);

	FloatingPointColor color;
#if defined(POMDOG_BYTEORDER_LITTLE_ENDIAN)
	color.r = static_cast<T>((rgba >> 24) & 0xff) / divisor; // 0x1234ABCD >> 24 = 0x12
	color.g = static_cast<T>((rgba >> 16) & 0xff) / divisor; // 0x1234ABCD >> 16 = 0x1234
	color.b = static_cast<T>((rgba >> 8) & 0xff) / divisor;  // 0x1234ABCD >>  8 = 0x1234AB
	color.a = static_cast<T>(rgba & 0xff) / divisor;         // 0x1234ABCD >>  0 = 0x1234ABCD
#elif defined(POMDOG_BYTEORDER_BIG_ENDIAN)
	color.a = static_cast<T>((rgba >> 24) & 0xff) / divisor;
	color.b = static_cast<T>((rgba >> 16) & 0xff) / divisor;
	color.g = static_cast<T>((rgba >> 8) & 0xff) / divisor;
	color.r = static_cast<T>(rgba & 0xff) / divisor;
#else
#	error "Byte order undefined or not supported."
#endif
	return color;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointColor<T> FloatingPointColor<T>::CreateHexARGB(std::uint32_t argb)
{
	constexpr T divisor = static_cast<T>(255);

	FloatingPointColor color;
#if defined(POMDOG_BYTEORDER_LITTLE_ENDIAN)
	color.a = static_cast<T>((argb >> 24) & 0xff) / divisor; // 0x1234ABCD >> 24 = 0x12
	color.r = static_cast<T>((argb >> 16) & 0xff) / divisor; // 0x1234ABCD >> 16 = 0x1234
	color.g = static_cast<T>((argb >> 8) & 0xff) / divisor;  // 0x1234ABCD >>  8 = 0x1234AB
	color.b = static_cast<T>(argb & 0xff) / divisor;         // 0x1234ABCD >>  0 = 0x1234ABCD
#elif defined(POMDOG_BYTEORDER_BIG_ENDIAN)
	color.b = static_cast<T>((argb >> 24) & 0xff) / divisor;
	color.g = static_cast<T>((argb >> 16) & 0xff) / divisor;
	color.r = static_cast<T>((argb >> 8) & 0xff) / divisor;
	color.a = static_cast<T>(argb & 0xff) / divisor;
#else
#	error "Byte order undefined or not supported."
#endif
	return color;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointColor<T> FloatingPointColor<T>::CreateLerp(
	FloatingPointColor const& source0, FloatingPointColor const& source1, T amount)
{
	static_assert(std::is_floating_point<decltype(amount)>::value, "The amout is floating point number");

	return FloatingPointColor(
		source0.r + amount * (source1.r - source0.r), 
		source0.g + amount * (source1.g - source0.g),
		source0.b + amount * (source1.b - source0.b), 
		source0.a + amount * (source1.a - source0.a));
}
//-----------------------------------------------------------------------
// explicit instantiations
template class FloatingPointColor<float>;

#if defined(DBL_MANT_DIG)
template class FloatingPointColor<double>;
#endif

#if defined(LDBL_MANT_DIG)
template class FloatingPointColor<long double>;
#endif

}// namespace Details
}// namespace Pomdog
