//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX2X2_HPP
#define POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX2X2_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../../Config/FundamentalTypes.hpp"
#include "ForwardDeclarations.hpp"
#include <type_traits>
#include <array>

namespace Pomdog {
namespace Details {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~Japanese
/// @brief 2x2 の浮動小数点数の行列を定義します。
template <typename T>
class FloatingPointMatrix2x2
{
public:
	static_assert(std::is_floating_point<T>::value, "T is floating point.");
	typedef T value_type;
		
	std::array<std::array<T, 2>, 2> m;

private:
	constexpr std::size_t RowSize(){ return 2; };
	constexpr std::size_t ColumnSize(){ return 2; };

public:
	// Constructors:
	FloatingPointMatrix2x2() = default;
	
	///@brief Copy constructor.
	FloatingPointMatrix2x2(FloatingPointMatrix2x2 const&) = default;
	
	///@brief Move constructor.
	FloatingPointMatrix2x2(FloatingPointMatrix2x2 &&) = default;
	
	///@brief Construct from floating-point values.
	FloatingPointMatrix2x2(T m00, T m01, T m10, T m11);
	
	virtual ~FloatingPointMatrix2x2() = default;
	
	// Assignment operators:
	///@brief Assignment.
	FloatingPointMatrix2x2 & operator=(FloatingPointMatrix2x2 const&) = default;
	///@brief Move assignment.
	FloatingPointMatrix2x2 & operator=(FloatingPointMatrix2x2 &&) = default;
	
	FloatingPointMatrix2x2 & operator*=(FloatingPointMatrix2x2 const& other);
	FloatingPointMatrix2x2 & operator+=(FloatingPointMatrix2x2 const& other);
	FloatingPointMatrix2x2 & operator-= (FloatingPointMatrix2x2 const& other);
	FloatingPointMatrix2x2 & operator*=(T scaleFactor);
	FloatingPointMatrix2x2 & operator/=(T scaleFactor);
	
	// Unary operators:
	FloatingPointMatrix2x2 operator+() const;
	FloatingPointMatrix2x2 operator-() const;

	// Binary operators:
	FloatingPointMatrix2x2 operator+(FloatingPointMatrix2x2 const& other) const;
	FloatingPointMatrix2x2 operator-(FloatingPointMatrix2x2 const& other) const;
	FloatingPointMatrix2x2 operator*(FloatingPointMatrix2x2 const& other) const;
	FloatingPointMatrix2x2 operator*(T scaleFactor) const;
	FloatingPointMatrix2x2 operator/(T scaleFactor) const;
	
	bool operator==(FloatingPointMatrix2x2 const& other) const;
	bool operator!=(FloatingPointMatrix2x2 const& other) const;

	// Function-call operators:
	T const& operator()(std::size_t row, std::size_t column) const;
	T & operator()(std::size_t row, std::size_t column);
	
	// Subscript operators:
	///@brief Array-indexing support.
	std::array<T, 2> const& operator[](std::size_t row) const;

	///@brief Array-indexing support.
	std::array<T, 2> & operator[](std::size_t row);

	///@~Japanese
	/// @brief 指定された行列を結合します。2x2 の正方行列の積を計算します。
	FloatingPointMatrix2x2 Concatenate(FloatingPointMatrix2x2 const& other) const;

	///@~Japanese
	/// @brief 2x2 の正方行列の積を計算します。
	FloatingPointMatrix2x2 Concatenate(T scaleFactor) const;

	///@~Japanese
	/// @brief 行列式を計算します。
	T Determinant() const;

	///@~English
	/// @brief Returns pointer to the first element.
	///@~Japanese
	/// @brief 最初の要素へのポインタを返します。
	T const* Data() const;

	///@~English
	/// @brief Returns pointer to the first element.
	///@~Japanese
	/// @brief 最初の要素へのポインタを返します。
	T* Data();
	
	///@~Japanese
	/// @brief 単位行列です。
	static FloatingPointMatrix2x2 const Identity;
};

template <typename T>
FloatingPointMatrix2x2<T> operator*(T scaleFactor, FloatingPointMatrix2x2<T> const& matrix);

/// @}
/// @}

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX2X2_HPP)
