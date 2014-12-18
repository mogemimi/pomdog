//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SANDBOXHELPER_254CE54A_4E5C_490B_A7A7_A0DF1AE34C85_HPP
#define POMDOG_SANDBOXHELPER_254CE54A_4E5C_490B_A7A7_A0DF1AE34C85_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog.Experimental/Gameplay2D/Camera2D.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform2D.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"

namespace TestApp {

struct SandboxHelper {
	static Matrix3x2 CreateViewMatrix2D(Transform2D const& transform, Camera2D const& camera)
	{
		POMDOG_ASSERT(camera.Zoom > 0);
		return Matrix3x2::CreateTranslation({-transform.Position.X, -transform.Position.Y})*
			Matrix3x2::CreateRotation(-transform.Rotation) *
			Matrix3x2::CreateScale(camera.Zoom);
	}

	static Matrix4x4 CreateViewMatrix(Transform2D const& transform, Camera2D const& camera)
	{
		POMDOG_ASSERT(camera.Zoom > 0);
		return Matrix4x4::CreateTranslation({-transform.Position.X, -transform.Position.Y, 1.0f})*
			Matrix4x4::CreateRotationZ(-transform.Rotation) *
			Matrix4x4::CreateScale({camera.Zoom, camera.Zoom, 1});
	}
};

}// namespace TestApp

#endif // !defined(POMDOG_SANDBOXHELPER_254CE54A_4E5C_490B_A7A7_A0DF1AE34C85_HPP)
