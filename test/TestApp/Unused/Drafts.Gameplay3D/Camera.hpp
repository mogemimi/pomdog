//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_CAMERA_6C3688A9_7B3B_4769_A599_EE6589634FB0_HPP
#define POMDOG_CAMERA_6C3688A9_7B3B_4769_A599_EE6589634FB0_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/Export.hpp>
#include <Pomdog/Math/detail/ForwardDeclarations.hpp>
#include <Pomdog/Math/Vector3.hpp>
#include <Pomdog/Math/Vector2.hpp>
#include <Pomdog/Math/Radian.hpp>
#include "ProjectionType.hpp"

namespace Pomdog {

class POMDOG_EXPORT Camera
{
public:
	Camera();

	ProjectionType ProjectionType() const;

	///@~Japanese
	/// @brief カメラのビューポートのサイズを取得します。
	Vector2 ViewportSize() const;
	
	///@~Japanese
	/// @brief カメラのビューポートのサイズを設定します。
	void ViewportSize(Vector2 const& viewportSize);
	
	///@~Japanese
	/// @brief 視錐台の最も近いクリップ面までの距離を取得します。
	float NearClipDistance() const;
	
	///@~Japanese
	/// @brief 視錐台の最も遠いクリップ面までの距離を取得します。
	float FarClipDistance() const;
	
	///@~Japanese
	/// @brief 視野角を取得します。
	Radian<float> FieldOfView() const;

	///@~Japanese
	/// @brief 視野角を取得します。
	void FieldOfView(Radian<float> const& fieldOfView);

	///@~Japanese
	/// @brief 視野角を取得します。
	void FieldOfView(Radian<float> && fieldOfView);

	///@~Japanese
	/// @brief アスペクト比を取得します。
	float Aspect() const;


	///@~Japanese
	/// @brief 透視投影モードに切り替えます。
	void Perspective(Radian<float> const& fieldOfView);
	
	///@~Japanese
	/// @brief 透視投影モードに切り替えます。
	void Perspective(Radian<float> && fieldOfView);
	
	///@~Japanese
	/// @brief 平行投影モードに切り替えます。
	void Orthographic();
	
	///@~Japanese
	/// @brief プロジェクション行列を作成します。
	Matrix4x4 CreateProjectionMatrix() const;
	
private:
	///@~Japanese
	/// @brief カメラのビューポートのサイズ
	Vector2 viewportSize;
	
	///@~Japanese
	/// @brief 視野角です。デフォルト値は MathConstants<float>::PiOver4() です。
	Radian<float> fieldOfView;
	
	///@~Japanese
	/// @brief 視錐台の最も近いクリップ面までの距離
	float nearClipDistance;
	
	///@~Japanese
	/// @brief 視錐台の最も遠いクリップ面までの距離
	float farClipDistance;
	
	Pomdog::ProjectionType projectionType;
};

}// namespace Pomdog

#endif // !defined(POMDOG_CAMERA_6C3688A9_7B3B_4769_A599_EE6589634FB0_HPP)
