//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Math/detail/FloatingPointMatrix2x2.hpp>
#include <cmath>
#include <limits>
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
namespace Details {

//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> const FloatingPointMatrix2x2<T>::Identity(
	1, 0,
	0, 1
);
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T>::FloatingPointMatrix2x2(T m00, T m01, T m10, T m11)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[1][0] = m10;
	m[1][1] = m11;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> &
FloatingPointMatrix2x2<T>::operator*=(FloatingPointMatrix2x2 const& other)
{
	return *this = this->Concatenate(other);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> &
FloatingPointMatrix2x2<T>::operator+=(FloatingPointMatrix2x2 const& other)
{
	m[0][0] += other.m[0][0];
	m[0][1] += other.m[0][1];
	m[1][0] += other.m[1][0];
	m[1][1] += other.m[1][1];
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> &
FloatingPointMatrix2x2<T>::operator-=(FloatingPointMatrix2x2 const& other)
{
	m[0][0] -= other.m[0][0];
	m[0][1] -= other.m[0][1];
	m[1][0] -= other.m[1][0];
	m[1][1] -= other.m[1][1];
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> &
FloatingPointMatrix2x2<T>::operator*=(T scaleFactor)
{
	m[0][0] *= scaleFactor;
	m[0][1] *= scaleFactor;
	m[1][0] *= scaleFactor;
	m[1][1] *= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> &
FloatingPointMatrix2x2<T>::operator/=(T scaleFactor)
{
	POMDOG_ASSERT(scaleFactor != static_cast<T>(0));
	auto const inverseDivider = static_cast<T>(1) / scaleFactor;
	m[0][0] *= inverseDivider;
	m[0][1] *= inverseDivider;
	m[1][0] *= inverseDivider;
	m[1][1] *= inverseDivider;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::operator+() const
{
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::operator-() const
{
	return FloatingPointMatrix2x2
		( -m[0][0]
		, -m[0][1]
		, -m[1][0]
		, -m[1][1]
		);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::operator+(FloatingPointMatrix2x2 const& other) const
{
	return FloatingPointMatrix2x2
		( m[0][0] + other.m[0][0]
		, m[0][1] + other.m[0][1]
		, m[1][0] + other.m[1][0]
		, m[1][1] + other.m[1][1]
		);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::operator-(FloatingPointMatrix2x2 const& other) const
{
	return FloatingPointMatrix2x2
		( m[0][0] - other.m[0][0]
		, m[0][1] - other.m[0][1]
		, m[1][0] - other.m[1][0]
		, m[1][1] - other.m[1][1]
		);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::operator*(FloatingPointMatrix2x2 const& other) const
{
	return this->Concatenate(other);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::operator*(T scaleFactor) const
{
	return this->Concatenate(scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::operator/(T scaleFactor) const
{
	auto const inverseDivider = static_cast<T>(1) / scaleFactor;
	return FloatingPointMatrix2x2
		( m[0][0] * inverseDivider
		, m[0][1] * inverseDivider
		, m[1][0] * inverseDivider
		, m[1][1] * inverseDivider
		);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointMatrix2x2<T>::operator==(FloatingPointMatrix2x2 const& other) const
{
	return (
		m[0][0] == other.m[0][0] && m[0][1] == other.m[0][1] &&
		m[1][0] == other.m[1][0] && m[1][1] == other.m[1][1]
	);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointMatrix2x2<T>::operator!=(FloatingPointMatrix2x2 const& other) const
{
	return (
		m[0][0] != other.m[0][0] || m[0][1] != other.m[0][1] ||
		m[1][0] != other.m[1][0] || m[1][1] != other.m[1][1]
	);
}
//-----------------------------------------------------------------------
template <typename T>
T & FloatingPointMatrix2x2<T>::operator()(std::size_t row, std::size_t column)
{
	POMDOG_ASSERT_MESSAGE(row < RowSize(), "row: out of range");
	POMDOG_ASSERT_MESSAGE(column < ColumnSize(), "column: out of range");
	return m[row][column];
}
//-----------------------------------------------------------------------
template <typename T>
T const& FloatingPointMatrix2x2<T>::operator()(std::size_t row, std::size_t column) const
{
	POMDOG_ASSERT_MESSAGE(row < RowSize(), "row: out of range");
	POMDOG_ASSERT_MESSAGE(column < ColumnSize(), "column: out of range");
	return m[row][column];
}
//-----------------------------------------------------------------------
template <typename T>
std::array<T, 2> const& FloatingPointMatrix2x2<T>::operator[](std::size_t row) const
{
	POMDOG_ASSERT_MESSAGE(row < RowSize(), "FloatingPointMatrix2x2, subscript out of range");
	return m[row];
}
//-----------------------------------------------------------------------
template <typename T>
std::array<T, 2> & FloatingPointMatrix2x2<T>::operator[](std::size_t row)
{
	POMDOG_ASSERT_MESSAGE(row < RowSize(), "FloatingPointMatrix2x2, subscript out of range");
	return m[row];
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::Concatenate(FloatingPointMatrix2x2 const& other) const
{
	return FloatingPointMatrix2x2
		( this->m[0][0] * other.m[0][0] + this->m[0][1] * other.m[1][0]
		, this->m[0][0] * other.m[0][1] + this->m[0][1] * other.m[1][1]
		, this->m[1][0] * other.m[0][0] + this->m[1][1] * other.m[1][0]
		, this->m[1][0] * other.m[0][1] + this->m[1][1] * other.m[1][1]
		);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::Concatenate(T scaleFactor) const
{
	return FloatingPointMatrix2x2
		( this->m[0][0] * scaleFactor
		, this->m[0][1] * scaleFactor
		, this->m[1][0] * scaleFactor
		, this->m[1][1] * scaleFactor
		);
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointMatrix2x2<T>::Determinant() const
{
	return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}
//-----------------------------------------------------------------------
template <typename T>
T const* FloatingPointMatrix2x2<T>::Data() const
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return m[0].data();
}
//-----------------------------------------------------------------------
template <typename T>
T* FloatingPointMatrix2x2<T>::Data()
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return m[0].data();
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointMatrix2x2<T> operator*(T scaleFactor, FloatingPointMatrix2x2<T> const& matrix)
{
	return matrix.Concatenate(scaleFactor);
}
//-----------------------------------------------------------------------
// explicit instantiations
template class FloatingPointMatrix2x2<float>;
template FloatingPointMatrix2x2<float> operator*<float>(float, FloatingPointMatrix2x2<float> const&);

#if defined(DBL_MANT_DIG)
template class FloatingPointMatrix2x2<double>;
template FloatingPointMatrix2x2<double> operator*<double>(double, FloatingPointMatrix2x2<double> const&);
#endif

#if defined(LDBL_MANT_DIG)
template class FloatingPointMatrix2x2<long double>;
template FloatingPointMatrix2x2<long double> operator*<long double>(long double, FloatingPointMatrix2x2<long double> const&);
#endif

}// namespace Details
}// namespace Pomdog
