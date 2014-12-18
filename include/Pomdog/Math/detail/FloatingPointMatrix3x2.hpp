//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX3X2_450DA908_CC1A_4EAC_9739_7E22E8453CDD_HPP
#define POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX3X2_450DA908_CC1A_4EAC_9739_7E22E8453CDD_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "ForwardDeclarations.hpp"
#include "Pomdog/Config/Export.hpp"
#include <cstddef>
#include <cstdint>
#include <cfloat>
#include <type_traits>
#include <array>

namespace Pomdog {
namespace Details {

///@~Japanese
/// @brief 3x2 の浮動小数点数の行列を定義します。
template <typename T>
class POMDOG_EXPORT FloatingPointMatrix3x2 {
public:
	static_assert(std::is_floating_point<T>::value, "T is floating point.");
	typedef T value_type;
	
	std::array<std::array<T, 2>, 3> m;

private:
	static constexpr std::size_t RowSize = 3;
	static constexpr std::size_t ColumnSize = 2;

public:
	// Constructors:
	FloatingPointMatrix3x2() = default;
	
	///@brief Copy constructor.
	FloatingPointMatrix3x2(FloatingPointMatrix3x2 const&) = default;
	
	///@brief Move constructor.
	FloatingPointMatrix3x2(FloatingPointMatrix3x2 &&) = default;
	
	///@brief Construct from floating-point values.
	FloatingPointMatrix3x2(
		T m00, T m01,
		T m10, T m11,
		T m20, T m21);
	
	// Assignment operators:
	///@brief Assignment.
	FloatingPointMatrix3x2 & operator=(FloatingPointMatrix3x2 const&) = default;
	///@brief Move assignment.
	FloatingPointMatrix3x2 & operator=(FloatingPointMatrix3x2 &&) = default;
	
	FloatingPointMatrix3x2 & operator*=(FloatingPointMatrix3x2 const& other);
	FloatingPointMatrix3x2 & operator+=(FloatingPointMatrix3x2 const& other);
	FloatingPointMatrix3x2 & operator-=(FloatingPointMatrix3x2 const& other);
	FloatingPointMatrix3x2 & operator*=(T scaleFactor);
	FloatingPointMatrix3x2 & operator/=(T scaleFactor);
	
	// Unary operators:
	FloatingPointMatrix3x2 operator+() const;
	FloatingPointMatrix3x2 operator-() const;
	
	// Binary operators:
	FloatingPointMatrix3x2 operator+(FloatingPointMatrix3x2 const& other) const;
	FloatingPointMatrix3x2 operator-(FloatingPointMatrix3x2 const& other) const;
	FloatingPointMatrix3x2 operator*(FloatingPointMatrix3x2 const& other) const;
	FloatingPointMatrix3x2 operator*(T scaleFactor) const;
	FloatingPointMatrix3x2 operator/(T scaleFactor) const;

	bool operator==(FloatingPointMatrix3x2 const& other) const;
	bool operator!=(FloatingPointMatrix3x2 const& other) const;
	
	// Function-call operators:
	T const& operator()(std::size_t row, std::size_t column) const;
	T & operator()(std::size_t row, std::size_t column);

	///@~Japanese
	/// @brief 行列式を計算します。
	T Determinant() const;

	///@~Japanese
	/// @brief 指定された行列を結合します。3x2 の行列の積を計算します。
	FloatingPointMatrix3x2 Concatenate(FloatingPointMatrix3x2 const& other) const;
	
	///@~Japanese
	/// @brief 3x2 の行列の積を計算します。
	FloatingPointMatrix3x2 Concatenate(T scaleFactor) const;

	///@~Japanese
	/// @brief 指定された行列の逆行列を作成します。
	static void
	Invert(FloatingPointMatrix3x2 const& matrix, FloatingPointMatrix3x2 & result);

	///@~Japanese
	/// @brief 指定された行列の逆行列を作成します。
	static FloatingPointMatrix3x2
	Invert(FloatingPointMatrix3x2 const& matrix);

	///@~Japanese
	/// @brief オフセットを指定して、平行移動行列を作成します。
	static void
	CreateTranslation(FloatingPointVector2<T> const& position, FloatingPointMatrix3x2 & result);

	///@~Japanese
	/// @brief オフセットを指定して、平行移動行列を作成します。
	static FloatingPointMatrix3x2
	CreateTranslation(FloatingPointVector2<T> const& position);

	///@~Japanese
	/// @brief スケーリング行列を作成します。
	static void
	CreateScale(T scale, FloatingPointMatrix3x2 & result);

	///@~Japanese
	/// @brief スケーリング行列を作成します。
	static FloatingPointMatrix3x2
	CreateScale(T scale);
	
	///@~Japanese
	/// @brief スケーリング行列を作成します。
	static void
	CreateScale(FloatingPointVector2<T> const& scale, FloatingPointMatrix3x2 & result);

	///@~Japanese
	/// @brief スケーリング行列を作成します。
	static FloatingPointMatrix3x2
	CreateScale(FloatingPointVector2<T> const& scale);
	
	///@~Japanese
	/// @brief z 軸を回転軸とした回転行列を作成します。
	static void
	CreateRotation(Radian<T> const& angle, FloatingPointMatrix3x2 & result);

	///@~Japanese
	/// @brief z 軸を回転軸とした回転行列を作成します。
	static FloatingPointMatrix3x2
	CreateRotation(Radian<T> const& angle);
	
	///@~Japanese
	/// @brief スキューイング行列を作成します。
	static void
	CreateSkew(FloatingPointVector2<T> const& skew, FloatingPointMatrix3x2 & result);

	///@~Japanese
	/// @brief スキューイング行列を作成します。
	static FloatingPointMatrix3x2
	CreateSkew(FloatingPointVector2<T> const& skew);

	///@~Japanese
	/// @brief 2つの行列間の対応する値を線形補間した行列を作成します。
	static void
	CreateLerp(FloatingPointMatrix3x2 const& source1, FloatingPointMatrix3x2 const& source2,
		T amount, FloatingPointMatrix3x2 & result);

	///@~Japanese
	/// @brief 2つの行列間の対応する値を線形補間した行列を作成します。
	static FloatingPointMatrix3x2
	CreateLerp(FloatingPointMatrix3x2 const& source1, FloatingPointMatrix3x2 const& source2,
		T amount);

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
	static FloatingPointMatrix3x2 const Identity;
};

template <typename T>
FloatingPointMatrix3x2<T> POMDOG_EXPORT
operator*(T scaleFactor, FloatingPointMatrix3x2<T> const& matrix);

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX3X2_450DA908_CC1A_4EAC_9739_7E22E8453CDD_HPP)
