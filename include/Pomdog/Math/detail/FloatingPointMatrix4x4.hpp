//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX4X4_DE77F1B1_1A17_4AFC_BCA2_232A4FB6ADD5_HPP
#define POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX4X4_DE77F1B1_1A17_4AFC_BCA2_232A4FB6ADD5_HPP

#if (_MSC_VER > 1000)
#	pragma once
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
/// @brief 4x4 の浮動小数点数の行列を定義します。
template <typename T>
class POMDOG_EXPORT FloatingPointMatrix4x4 {
public:
	static_assert(std::is_floating_point<T>::value, "T is floating point.");
	typedef T value_type;
	
	//union {
	//	std::array<std::array<T, 4>, 4> m;
	//#if defined __i386__ || defined __x86_64__
	//	std::array<__m128, 4>
	//#endif
	//};
	std::array<std::array<T, 4>, 4> m;
	
private:
	static constexpr std::size_t RowSize = 4;
	static constexpr std::size_t ColumnSize = 4;

public:
	// Constructors:
	FloatingPointMatrix4x4() = default;
	
	///@brief Copy constructor.
	FloatingPointMatrix4x4(FloatingPointMatrix4x4 const&) = default;
	
	///@brief Move constructor.
	FloatingPointMatrix4x4(FloatingPointMatrix4x4 &&) = default;
	
	///@brief Construct from floating-point values.
	FloatingPointMatrix4x4(
		T m00, T m01, T m02, T m03,
		T m10, T m11, T m12, T m13,
		T m20, T m21, T m22, T m23,
		T m30, T m31, T m32, T m33);
	
	// Assignment operators:
	FloatingPointMatrix4x4 & operator=(FloatingPointMatrix4x4 const&) = default;
	FloatingPointMatrix4x4 & operator=(FloatingPointMatrix4x4 &&) = default;
	FloatingPointMatrix4x4 & operator*=(FloatingPointMatrix4x4 const&);
	FloatingPointMatrix4x4 & operator+=(FloatingPointMatrix4x4 const&);
	FloatingPointMatrix4x4 & operator-=(FloatingPointMatrix4x4 const&);
	FloatingPointMatrix4x4 & operator*=(T scaleFactor);
	FloatingPointMatrix4x4 & operator/=(T scaleFactor);

	// Unary operators:
	FloatingPointMatrix4x4 operator+() const;
	FloatingPointMatrix4x4 operator-() const;

	// Binary operators:
	FloatingPointMatrix4x4 operator+(FloatingPointMatrix4x4 const&) const;
	FloatingPointMatrix4x4 operator-(FloatingPointMatrix4x4 const&) const;
	FloatingPointMatrix4x4 operator*(FloatingPointMatrix4x4 const&) const;
	FloatingPointMatrix4x4 operator*(T scaleFactor) const;
	FloatingPointMatrix4x4 operator/(T scaleFactor) const;

	bool operator==(FloatingPointMatrix4x4 const&) const;
	bool operator!=(FloatingPointMatrix4x4 const&) const;

	// Function-call operators:
	T const& operator()(std::size_t row, std::size_t column) const;
	T & operator()(std::size_t row, std::size_t column);
	
	///@~Japanese
	/// @brief 平行移動ベクトルを設定します。
	void SetTranslation(FloatingPointVector3<T> const&);

	///@~Japanese
	/// @brief 平行移動ベクトルを取得します。
	FloatingPointVector3<T> GetTranslation() const;
		
	///@~Japanese
	/// @brief スケールベクトルを設定します。
	void SetScale(FloatingPointVector3<T> const&);

	///@~Japanese
	/// @brief スケールベクトルを取得します。
	FloatingPointVector3<T> GetScale() const;
		
	///@~Japanese
	/// @brief 行列式を計算します。
	T Determinant() const;
		
	///@~Japanese
	/// @brief 指定された行列を結合します。4x4 の正方行列の積を計算します。
	FloatingPointMatrix4x4 Concatenate(FloatingPointMatrix4x4 const&) const;
		
	///@~Japanese
	/// @brief 4x4 の正方行列の積を計算します。
	FloatingPointMatrix4x4 Concatenate(T scaleFactor) const;

	///@~Japanese
	/// @brief 3x3 の部分行列（小行列式）を取得します。
	FloatingPointMatrix3x3<T> Minor3x3(std::size_t row, std::size_t column) const;

	///@~Japanese
	/// @brief 余因子行列を計算します。
	static FloatingPointMatrix4x4
	Adjoint(FloatingPointMatrix4x4 const& matrix);

	///@~Japanese
	/// @brief 転置行列を計算します。
	static void
	Transpose(FloatingPointMatrix4x4 const& matrix, FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief 転置行列を計算します。
	static FloatingPointMatrix4x4
	Transpose(FloatingPointMatrix4x4 const& matrix);

	///@~Japanese
	/// @brief 指定された行列の逆行列を作成します。
	static void
	Invert(FloatingPointMatrix4x4 const& matrix, FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief 指定された行列の逆行列を作成します。
	static FloatingPointMatrix4x4
	Invert(FloatingPointMatrix4x4 const& matrix);

	///@~Japanese
	/// @brief 2つの行列間の対応する値を線形補間した行列を作成します。
	static void
	Lerp(FloatingPointMatrix4x4 const& source1, FloatingPointMatrix4x4 const& source2, T amount,
		FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief 2つの行列間の対応する値を線形補間した行列を作成します。
	static FloatingPointMatrix4x4
	Lerp(FloatingPointMatrix4x4 const& source1, FloatingPointMatrix4x4 const& source2, T amount);

	///@~Japanese
	/// @brief オフセットを指定して、平行移動行列を作成します。
	static void
	CreateTranslation(FloatingPointVector3<T> const& position, FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief オフセットを指定して、平行移動行列を作成します。
	static FloatingPointMatrix4x4
	CreateTranslation(FloatingPointVector3<T> const& position);

	///@~Japanese
	/// @brief スケーリング行列を作成します。
	static void
	CreateScale(T scale, FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief スケーリング行列を作成します。
	static FloatingPointMatrix4x4
	CreateScale(T scale);

	///@~Japanese
	/// @brief スケーリング行列を作成します。
	static void
	CreateScale(FloatingPointVector3<T> const& scale, FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief スケーリング行列を作成します。
	static FloatingPointMatrix4x4
	CreateScale(FloatingPointVector3<T> const& scale);

	///@~Japanese
	/// @brief x 軸を回転軸とした回転行列を作成します。
	static void
	CreateRotationX(Radian<T> const& angle, FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief x 軸を回転軸とした回転行列を作成します。
	static FloatingPointMatrix4x4
	CreateRotationX(Radian<T> const& angle);

	///@~Japanese
	/// @brief y 軸を回転軸とした回転行列を作成します。
	static void
	CreateRotationY(Radian<T> const& angle, FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief y 軸を回転軸とした回転行列を作成します。
	static FloatingPointMatrix4x4
	CreateRotationY(Radian<T> const& angle);

	///@~Japanese
	/// @brief z 軸を回転軸とした回転行列を作成します。
	static void
	CreateRotationZ(Radian<T> const& angle, FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief z 軸を回転軸とした回転行列を作成します。
	static FloatingPointMatrix4x4
	CreateRotationZ(Radian<T> const& angle);

	///@~Japanese
	/// @brief クォータニオンから行列を作成します。
	static void
	CreateFromQuaternion(FloatingPointQuaternion<T> const& quaternion, FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief クォータニオンから行列を作成します。
	static FloatingPointMatrix4x4
	CreateFromQuaternion(FloatingPointQuaternion<T> const& quaternion);

	///@~Japanese
	/// @brief 左手座標系ビュー行列を作成します。
	/// @param eye カメラの位置
	/// @param at カメラを向ける対象の位置
	/// @param up カメラの視点から上方向を表すベクトル
	static void
	CreateLookAtLH(FloatingPointVector3<T> const& eye, FloatingPointVector3<T> const& at,
		FloatingPointVector3<T> const& up, FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief 左手座標系ビュー行列を作成します。
	/// @param eye カメラの位置
	/// @param at カメラを向ける対象の位置
	/// @param up カメラの視点から上方向を表すベクトル
	static FloatingPointMatrix4x4
	CreateLookAtLH(FloatingPointVector3<T> const& eye, FloatingPointVector3<T> const& at,
		FloatingPointVector3<T> const& up);

	///@~Japanese
	/// @brief 右手座標系ビュー行列を作成します。
	/// @param eye カメラの位置
	/// @param at カメラを向ける対象の位置
	/// @param up カメラの視点から上方向を表すベクトル
	static void
	CreateLookAtRH(FloatingPointVector3<T> const& eye, FloatingPointVector3<T> const& at,
		FloatingPointVector3<T> const& up, FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief 右手座標系ビュー行列を作成します。
	/// @param eye カメラの位置
	/// @param at カメラを向ける対象の位置
	/// @param up カメラの視点から上方向を表すベクトル
	static FloatingPointMatrix4x4
	CreateLookAtRH(FloatingPointVector3<T> const& eye, FloatingPointVector3<T> const& at,
		FloatingPointVector3<T> const& up);

	///@~Japanese
	/// @brief 左手座標系パースペクティブ射影行列を作成します。
	/// @param width ビューボリュームの幅
	/// @param height ビューボリュームの高さ
	/// @param zNear 近くのビュー平面(near plane)の Z 値
	/// @param zFar 遠いのビュー平面(far plane)の Z 値
	static void
	CreatePerspectiveLH(T width, T height, T zNear, T zFar, FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief 右手座標系パースペクティブ射影行列を作成します。
	static void
	CreatePerspectiveRH(T width, T height, T zNear, T zFar, FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief 視野に基づいて、左手座標系パースペクティブ射影行列を作成します。
	static void
	CreatePerspectiveFieldOfViewLH(Radian<T> const& fovy, T aspect, T zNear, T zFar,
		FloatingPointMatrix4x4 & result);
		
	///@~Japanese
	/// @brief 視野に基づいて、左手座標系パースペクティブ射影行列を作成します。
	static FloatingPointMatrix4x4
	CreatePerspectiveFieldOfViewLH(Radian<T> const& fovy, T aspect, T zNear, T zFar);
	
	///@~Japanese
	/// @brief 視野に基づいて、右手座標系パースペクティブ射影行列を作成します。
	static void
	CreatePerspectiveFieldOfViewRH(Radian<T> const& fovy, T aspect, T zNear, T zFar,
		FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief 視野に基づいて、右手座標系パースペクティブ射影行列を作成します。
	static FloatingPointMatrix4x4
	CreatePerspectiveFieldOfViewRH(Radian<T> const& fovy, T aspect, T zNear, T zFar);

	///@~Japanese
	/// @brief 細かく指定された左手座標系パースペクティブ射影行列を作成します。
	static void
	CreatePerspectiveOffCenterLH(T left, T right, T bottom, T top, T zNear, T zFar,
		FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief 細かく指定された左手座標系パースペクティブ射影行列を作成します。
	static FloatingPointMatrix4x4
	CreatePerspectiveOffCenterLH(T left, T right, T bottom, T top, T zNear, T zFar);

	///@~Japanese
	/// @brief 細かく指定された右手座標系パースペクティブ射影行列を作成します。
	static void
	CreatePerspectiveOffCenterRH(T left, T right, T bottom, T top, T zNear, T zFar,
		FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief 細かく指定された右手座標系パースペクティブ射影行列を作成します。
	static FloatingPointMatrix4x4
	CreatePerspectiveOffCenterRH(T left, T right, T bottom, T top, T zNear, T zFar);

	///@~Japanese
	/// @brief 左手座標系正射影行列を作成します。
	static void
	CreateOrthographicLH(T width, T height, T zNearPlane, T zFarPlane,
		FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief 左手座標系正射影行列を作成します。
	static FloatingPointMatrix4x4
	CreateOrthographicLH(T width, T height, T zNearPlane, T zFarPlane);

	///@~Japanese
	/// @brief 右手座標系正射影行列を作成します。
	static void
	CreateOrthographicRH(T width, T height, T zNearPlane, T zFarPlane,
		FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief 右手座標系正射影行列を作成します。
	static FloatingPointMatrix4x4
	CreateOrthographicRH(T width, T height, T zNearPlane, T zFarPlane);

	///@~Japanese
	/// @brief 細かく指定された左手座標系正射影行列を作成します。
	static void
	CreateOrthographicOffCenterLH(T left, T right, T bottom, T top, T zNearPlane, T zFarPlane,
		FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief 細かく指定された左手座標系正射影行列を作成します。
	static FloatingPointMatrix4x4
	CreateOrthographicOffCenterLH(T left, T right, T bottom, T top, T zNearPlane, T zFarPlane);

	///@~Japanese
	/// @brief 細かく指定された右手座標系正射影行列を作成します。
	static void
	CreateOrthographicOffCenterRH(T left, T right, T bottom, T top, T zNearPlane, T zFarPlane,
		FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief 細かく指定された右手座標系正射影行列を作成します。
	static FloatingPointMatrix4x4
	CreateOrthographicOffCenterRH(T left, T right, T bottom, T top, T zNearPlane, T zFarPlane);

	///@~Japanese
	/// @brief 任意の軸を回転軸とした回転行列を作成します。
	static void
	CreateFromAxisAngle(FloatingPointVector3<T> const& axis, Radian<T> const& angle, FloatingPointMatrix4x4 & result);

	///@~Japanese
	/// @brief 任意の軸を回転軸とした回転行列を作成します。
	static FloatingPointMatrix4x4
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
	static FloatingPointMatrix4x4 const Identity;
};

template <typename T>
FloatingPointMatrix4x4<T> POMDOG_EXPORT
operator*(T scaleFactor, FloatingPointMatrix4x4<T> const& matrix);

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MATH_DETAIL_FLOATINGPOINTMATRIX4X4_DE77F1B1_1A17_4AFC_BCA2_232A4FB6ADD5_HPP)
