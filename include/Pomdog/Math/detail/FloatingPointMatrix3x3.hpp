//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX3X3_69EBDB78_9A8B_47F1_A99C_D568F466F249_HPP
#define POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX3X3_69EBDB78_9A8B_47F1_A99C_D568F466F249_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../../Config/Export.hpp"
#include "../../Config/FundamentalTypes.hpp"
#include "ForwardDeclarations.hpp"
#include "../Radian.hpp"
#include <type_traits>
#include <array>

namespace Pomdog {
namespace Details {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~Japanese
/// @brief 4x4 の浮動小数点数の行列を定義します。
template <typename T>
class POMDOG_EXPORT FloatingPointMatrix3x3
{
public:
	static_assert(std::is_floating_point<T>::value, "T is floating point.");
	typedef T value_type;
	
	std::array<std::array<T, 3>, 3> m;

private:
	constexpr std::size_t RowSize(){ return 3; };
	constexpr std::size_t ColumnSize(){ return 3; };

public:
	// Constructors:
	FloatingPointMatrix3x3() = default;
	
	///@brief Copy constructor.
	FloatingPointMatrix3x3(FloatingPointMatrix3x3 const&) = default;
	
	///@brief Move constructor.
	FloatingPointMatrix3x3(FloatingPointMatrix3x3 &&) = default;
	
	///@brief Construct from floating-point values.
	FloatingPointMatrix3x3(
		T m00, T m01, T m02, 
		T m10, T m11, T m12, 
		T m20, T m21, T m22
	);
	
	~FloatingPointMatrix3x3() = default;
	
	// Assignment operators:
	///@brief Assignment.
	FloatingPointMatrix3x3 & operator=(FloatingPointMatrix3x3 const&) = default;
	///@brief Move assignment.
	FloatingPointMatrix3x3 & operator=(FloatingPointMatrix3x3 &&) = default;
	
	FloatingPointMatrix3x3 & operator*=(FloatingPointMatrix3x3 const& other);
	FloatingPointMatrix3x3 & operator+=(FloatingPointMatrix3x3 const& other);
	FloatingPointMatrix3x3 & operator-=(FloatingPointMatrix3x3 const& other);
	FloatingPointMatrix3x3 & operator*=(T scaleFactor);
	FloatingPointMatrix3x3 & operator/=(T scaleFactor);
	
	// Unary operators:
	FloatingPointMatrix3x3 operator+() const;
	FloatingPointMatrix3x3 operator-() const;
	
	// Binary operators:
	FloatingPointMatrix3x3 operator+(FloatingPointMatrix3x3 const& other) const;
	FloatingPointMatrix3x3 operator-(FloatingPointMatrix3x3 const& other) const;
	FloatingPointMatrix3x3 operator*(FloatingPointMatrix3x3 const& other) const;
	FloatingPointMatrix3x3 operator*(T scaleFactor) const;
	FloatingPointMatrix3x3 operator/(T scaleFactor) const;

	bool operator == (FloatingPointMatrix3x3 const& other) const;
	bool operator != (FloatingPointMatrix3x3 const& other) const;
	
	// Function-call operators:
	T const& operator()(std::size_t row, std::size_t column) const;
	T & operator()(std::size_t row, std::size_t column);

	// Subscript operators:
	///@brief Array-indexing support.
	std::array<T, 3> const& operator[](std::size_t row) const;
	
	///@brief Array-indexing support.
	std::array<T, 3> & operator[](std::size_t row);

	///@~Japanese
	/// @brief スケールベクトルを設定します。
	void SetScale(FloatingPointVector3<T> const& scale);

	///@~Japanese
	/// @brief スケールベクトルを取得します。
	FloatingPointVector3<T> GetScale() const;

	///@~Japanese
	/// @brief 転置行列を計算します。
	FloatingPointMatrix3x3 Transpose() const;

	///@~Japanese
	/// @brief 行列式を計算します。
	T Determinant() const;

	///@~Japanese
	/// @brief 余因子行列を計算します。
	FloatingPointMatrix3x3 Adjoint() const;

	///@~Japanese
	/// @brief 指定された行列を結合します。3x3 の正方行列の積を計算します。
	FloatingPointMatrix3x3 Concatenate(FloatingPointMatrix3x3 const& other) const;
	
	///@~Japanese
	/// @brief 3x3 の正方行列の積を計算します。
	FloatingPointMatrix3x3 Concatenate(T scaleFactor) const;

	///@~Japanese
	/// @brief 2x2 の部分行列（小行列式）を取得します。
	FloatingPointMatrix2x2<T> Minor2x2(std::size_t row, std::size_t column) const;

	///@~Japanese
	/// @brief 指定された行列の逆行列を作成します。
	static void
	Invert(FloatingPointMatrix3x3 const& matrix, FloatingPointMatrix3x3 & result);

	///@~Japanese
	/// @brief 指定された行列の逆行列を作成します。
	static FloatingPointMatrix3x3
	Invert(FloatingPointMatrix3x3 const& matrix);

	///@~Japanese
	/// @brief オフセットを指定して、平行移動行列を作成します。
	static void
	CreateTranslation(FloatingPointVector2<T> const& position, FloatingPointMatrix3x3 & result);

	///@~Japanese
	/// @brief オフセットを指定して、平行移動行列を作成します。
	static FloatingPointMatrix3x3
	CreateTranslation(FloatingPointVector2<T> const& position);

	///@~Japanese
	/// @brief スケーリング行列を作成します。
	static void
	CreateScale(FloatingPointVector3<T> const& scale, FloatingPointMatrix3x3 & result);

	///@~Japanese
	/// @brief スケーリング行列を作成します。
	static FloatingPointMatrix3x3
	CreateScale(FloatingPointVector3<T> const& scale);

	///@~Japanese
	/// @brief x 軸を回転軸とした回転行列を作成します。
	static void
	CreateRotationX(Radian<T> const& angle, FloatingPointMatrix3x3 & result);

	///@~Japanese
	/// @brief x 軸を回転軸とした回転行列を作成します。
	static FloatingPointMatrix3x3
	CreateRotationX(Radian<T> const& angle);

	///@~Japanese
	/// @brief y 軸を回転軸とした回転行列を作成します。
	static void
	CreateRotationY(Radian<T> const& angle, FloatingPointMatrix3x3 & result);
	
	///@~Japanese
	/// @brief y 軸を回転軸とした回転行列を作成します。
	static FloatingPointMatrix3x3
	CreateRotationY(Radian<T> const& angle);

	///@~Japanese
	/// @brief z 軸を回転軸とした回転行列を作成します。
	static void
	CreateRotationZ(Radian<T> const& angle, FloatingPointMatrix3x3 & result);

	///@~Japanese
	/// @brief z 軸を回転軸とした回転行列を作成します。
	static FloatingPointMatrix3x3
	CreateRotationZ(Radian<T> const& angle);

	///@~Japanese
	/// @brief クォータニオンから行列を作成します。
	static void
	CreateFromQuaternion(FloatingPointQuaternion<T> const& quaternion, FloatingPointMatrix3x3 & result);
	
	///@~Japanese
	/// @brief クォータニオンから行列を作成します。
	static FloatingPointMatrix3x3
	CreateFromQuaternion(FloatingPointQuaternion<T> const& quaternion);

	///@~Japanese
	/// @brief 2つの行列間の対応する値を線形補間した行列を作成します。
	static void
	CreateLerp(FloatingPointMatrix3x3 const& source1, FloatingPointMatrix3x3 const& source2,
		T amount, FloatingPointMatrix3x3 & result);

	///@~Japanese
	/// @brief 2つの行列間の対応する値を線形補間した行列を作成します。
	static FloatingPointMatrix3x3
	CreateLerp(FloatingPointMatrix3x3 const& source1, FloatingPointMatrix3x3 const& source2,
		T amount);

	///@~Japanese
	/// @brief 任意の軸を回転軸とした回転行列を作成します。
	static void
	CreateFromAxisAngle(FloatingPointVector3<T> const& axis, Radian<T> const& angle, FloatingPointMatrix3x3 & result);

	///@~Japanese
	/// @brief 任意の軸を回転軸とした回転行列を作成します。
	static FloatingPointMatrix3x3
	CreateFromAxisAngle(FloatingPointVector3<T> const& axis, Radian<T> const& angle);

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
	static FloatingPointMatrix3x3 const Identity;
};

template <typename T>
FloatingPointMatrix3x3<T> operator*(T scaleFactor, FloatingPointMatrix3x3<T> const& matrix);

/// @}
/// @}

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX3X3_69EBDB78_9A8B_47F1_A99C_D568F466F249_HPP)
