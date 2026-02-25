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
    int topLeftX;

    /// The y coordinate of the upper-left corner of the viewport.
    int topLeftY;

    /// The width of the viewport, in pixels.
    int width;

    /// The height of the viewport, in pixels.
    int height;

    /// The minimum depth of the clip volume. It must be between 0.0 and 1.0.
    float minDepth;

    /// The maximum depth of the clip volume. It must be between 0.0 and 1.0.
    float maxDepth;

public:
    Viewport() noexcept = default;

    explicit Viewport(const Rect2D& bounds) noexcept;

    Viewport(int x, int y, int width, int height) noexcept;

    Viewport(int x, int y, int width, int height, float minDepth, float maxDepth) noexcept;

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
    [[nodiscard]] float
    getAspectRatio() const noexcept;
};

} // namespace pomdog::gpu
