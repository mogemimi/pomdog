// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
class F32Rect2D;
class Matrix4x4;
class Ray;
class Vector2;
class Vector3;
} // namespace pomdog

namespace pomdog::math {

/// Projects the specified source vector from model space into screen space.
[[nodiscard]] POMDOG_EXPORT Vector3
project(
    const F32Rect2D& viewport,
    f32 minDepth,
    f32 maxDepth,
    const Vector3& source,
    const Matrix4x4& worldViewProjection) noexcept;

/// Unprojects the specified source vector from screen space into model space.
[[nodiscard]] POMDOG_EXPORT Vector3
unproject(
    const F32Rect2D& viewport,
    f32 minDepth,
    f32 maxDepth,
    const Vector3& source,
    const Matrix4x4& inverseWorldViewProjection) noexcept;

/// Creates a ray from a screen point into the 3D scene.
[[nodiscard]] POMDOG_EXPORT Ray
screenPointToRay(
    const Vector2& screenPoint,
    const Vector3& cameraPosition,
    const F32Rect2D& viewport,
    f32 minDepth,
    f32 maxDepth,
    const Matrix4x4& inverseViewProjection,
    bool isOrthoProjection);

} // namespace pomdog::math
