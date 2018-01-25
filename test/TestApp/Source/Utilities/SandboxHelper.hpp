// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

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
