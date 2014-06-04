//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SANDBOXHELPER_254CE54A_4E5C_490B_A7A7_A0DF1AE34C85_HPP
#define POMDOG_SANDBOXHELPER_254CE54A_4E5C_490B_A7A7_A0DF1AE34C85_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>

namespace TestApp {

struct SandboxHelper {
	static Matrix3x3 CreateViewMatrix2D(Transform2D const& transform, Camera2D const& camera)
	{
		return Matrix3x3::CreateTranslation({-transform.Position.X, -transform.Position.Y})*
			Matrix3x3::CreateRotationZ(-transform.Rotation) *
			Matrix3x3::CreateScale({camera.Zoom(), camera.Zoom(), 1});
	}

	static Matrix4x4 CreateViewMatrix3D(Transform2D const& transform, Camera2D const& camera)
	{
		return Matrix4x4::CreateTranslation({-transform.Position.X, -transform.Position.Y, 1.0f})*
			Matrix4x4::CreateRotationZ(-transform.Rotation) *
			Matrix4x4::CreateScale({camera.Zoom(), camera.Zoom(), 1});
	}
};

}// namespace TestApp

#endif // !defined(POMDOG_SANDBOXHELPER_254CE54A_4E5C_490B_A7A7_A0DF1AE34C85_HPP)
