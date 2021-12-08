// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.hpp"
#include "pomdog/math/forward_declarations.hpp"

namespace pomdog {

class POMDOG_EXPORT Viewport final {
public:
    /// The x coordinate of the upper-left corner of the viewport.
    int TopLeftX;

    /// The y coordinate of the upper-left corner of the viewport.
    int TopLeftY;

    /// The width of the viewport, in pixels.
    int Width;

    /// The height of the viewport, in pixels.
    int Height;

    /// The minimum depth of the clip volume. It must be between 0.0 and 1.0.
    float MinDepth;

    /// The maximum depth of the clip volume. It must be between 0.0 and 1.0.
    float MaxDepth;

public:
    Viewport() noexcept = default;

    explicit Viewport(const Rectangle& bounds) noexcept;

    Viewport(int x, int y, int width, int height) noexcept;

    Viewport(int x, int y, int width, int height,
        float minDepth, float maxDepth) noexcept;

    /// Projects a position from object space into screen space.
    Vector3 Project(
        const Vector3& source,
        const Matrix4x4& worldViewProjection) const;

    /// Projects a position from screen space into world space.
    Vector3 Unproject(
        const Vector3& source,
        const Matrix4x4& worldViewProjection) const;

    /// Gets the boundary of this viewport.
    [[nodiscard]] Rectangle GetBounds() const noexcept;

    /// Gets the aspect ratio used by the viewport, which is width / height.
    [[nodiscard]] float GetAspectRatio() const noexcept;
};

} // namespace pomdog
