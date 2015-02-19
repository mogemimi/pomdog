//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX2X2_D1549734_DE89_4B72_BB99_7131858C8133_HPP
#define POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX2X2_D1549734_DE89_4B72_BB99_7131858C8133_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstddef>
#include <cstdint>
#include <cfloat>
#include <type_traits>
#include <array>

namespace Pomdog {
namespace Details {

///@~Japanese
/// @brief 2x2 の浮動小数点数の行列を定義します。
template <typename T>
class POMDOG_EXPORT FloatingPointMatrix2x2 {
public:
	static_assert(std::is_floating_point<T>::value, "T is floating point.");
	typedef T value_type;

	std::array<std::array<T, 2>, 2> m;

private:
	static constexpr std::size_t RowSize = 2;
	static constexpr std::size_t ColumnSize = 2;

public:
	// Constructors:
	FloatingPointMatrix2x2() = default;

	///@brief Copy constructor.
	FloatingPointMatrix2x2(FloatingPointMatrix2x2 const&) = default;

	///@brief Move constructor.
	FloatingPointMatrix2x2(FloatingPointMatrix2x2 &&) = default;

	///@brief Construct from floating-point values.
	FloatingPointMatrix2x2(T m00, T m01, T m10, T m11);

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
FloatingPointMatrix2x2<T> POMDOG_EXPORT
operator*(T scaleFactor, FloatingPointMatrix2x2<T> const& matrix);

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX2X2_D1549734_DE89_4B72_BB99_7131858C8133_HPP)
