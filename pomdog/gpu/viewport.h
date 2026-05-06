// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
class Rect2D;
class Vector3;
class Matrix4x4;
} // namespace pomdog

namespace pomdog::gpu {

class POMDOG_EXPORT Viewport final {
public:
    /// The x coordinate of the upper-left corner of the viewport.
    i32 topLeftX;

    /// The y coordinate of the upper-left corner of the viewport.
    i32 topLeftY;

    /// The width of the viewport, in pixels.
    i32 width;

    /// The height of the viewport, in pixels.
    i32 height;

    /// The minimum depth of the clip volume. It must be between 0.0 and 1.0.
    f32 minDepth;

    /// The maximum depth of the clip volume. It must be between 0.0 and 1.0.
    f32 maxDepth;

public:
    Viewport() noexcept = default;

    explicit Viewport(const Rect2D& bounds) noexcept;

    Viewport(i32 x, i32 y, i32 width, i32 height) noexcept;

    Viewport(i32 x, i32 y, i32 width, i32 height, f32 minDepth, f32 maxDepth) noexcept;

    /// Projects a position from object space into screen space.
    [[nodiscard]] Vector3
    project(const Vector3& source, const Matrix4x4& worldViewProjection) const;

    /// Projects a position from screen space into world space.
    [[nodiscard]] Vector3
    unproject(const Vector3& source, const Matrix4x4& worldViewProjection) const;

    /// Gets the boundary of this viewport.
    [[nodiscard]] Rect2D
    getBounds() const noexcept;

    /// Gets the aspect ratio used by the viewport, which is width / height.
    [[nodiscard]] f32
    getAspectRatio() const noexcept;
};

} // namespace pomdog::gpu
