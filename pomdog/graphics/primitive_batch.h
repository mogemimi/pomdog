// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class CommandList;
class GraphicsDevice;
struct DepthStencilDesc;
struct RasterizerDesc;
} // namespace pomdog::gpu

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog {
class BoundingBox;
class Color;
class Error;
class Matrix3x2;
class Matrix4x4;
class Rect2D;
class Vector2;
class Vector3;
class Vector4;
template <typename T>
class Radian;
} // namespace pomdog

namespace pomdog {

/// Holds the pipeline state (shaders, blend state, etc.) for PrimitiveBatch.
class POMDOG_EXPORT PrimitivePipeline {
public:
    virtual ~PrimitivePipeline();
};

/// Renders filled primitive shapes (triangles, circles, rectangles, etc.) in a scene.
///
/// Usage pattern:
/// ```cpp
/// primitiveBatch->reset();
/// primitiveBatch->setTransform(projectionMatrix);
/// primitiveBatch->drawCircle(position, radius, segments, color);
/// primitiveBatch->flush(commandList, primitivePipeline);
/// primitiveBatch->submit(graphicsDevice);
/// ```
class POMDOG_EXPORT PrimitiveBatch {
public:
    virtual ~PrimitiveBatch();

    /// Resets the batch state, clearing any previously batched vertices.
    virtual void
    reset() = 0;

    /// Sets the transformation matrix to be applied to all primitives in the batch.
    ///
    /// Uses `transformMatrix` for the view-projection matrix.
    virtual void
    setTransform(const Matrix4x4& transformMatrix) = 0;

    /// Draws a filled arc (pie shape).
    ///
    /// Uses `position` for center of the arc.
    /// Uses `radius` for radius of the arc.
    /// Uses `startAngle` for starting angle in radians.
    /// Uses `arcAngle` for sweep angle in radians.
    /// Uses `segments` for number of triangle segments used to approximate the arc.
    /// Uses `color` for fill color.
    virtual void
    drawArc(
        const Vector2& position,
        f32 radius,
        const Radian<f32>& startAngle,
        const Radian<f32>& arcAngle,
        i32 segments,
        const Color& color) = 0;

    /// Draws a filled box from a bounding box.
    ///
    /// Uses `box` for the axis-aligned bounding box.
    /// Uses `color` for fill color.
    virtual void
    drawBox(
        const BoundingBox& box,
        const Color& color) = 0;

    /// Draws a filled box at the given position and scale.
    ///
    /// Uses `position` for position of the box origin.
    /// Uses `scale` for size of the box along each axis.
    /// Uses `color` for fill color.
    virtual void
    drawBox(
        const Vector3& position,
        const Vector3& scale,
        const Color& color) = 0;

    /// Draws a filled box at the given position, scale, and origin pivot.
    ///
    /// Uses `position` for position of the box.
    /// Uses `scale` for size of the box along each axis.
    /// Uses `originPivot` for origin pivot point (0..1 per axis).
    /// Uses `color` for fill color.
    virtual void
    drawBox(
        const Vector3& position,
        const Vector3& scale,
        const Vector3& originPivot,
        const Color& color) = 0;

    /// Draws a filled circle in 2D.
    ///
    /// Uses `position` for center of the circle.
    /// Uses `radius` for radius.
    /// Uses `segments` for number of triangle segments.
    /// Uses `color` for fill color.
    virtual void
    drawCircle(
        const Vector2& position,
        f32 radius,
        i32 segments,
        const Color& color) = 0;

    /// Draws a filled circle in 3D.
    ///
    /// Uses `position` for center of the circle.
    /// Uses `radius` for radius.
    /// Uses `segments` for number of triangle segments.
    /// Uses `color` for fill color.
    virtual void
    drawCircle(
        const Vector3& position,
        f32 radius,
        i32 segments,
        const Color& color) = 0;

    /// Draws a thick line between two 2D points.
    ///
    /// Uses `start` for start point.
    /// Uses `end` for end point.
    /// Uses `color` for line color.
    /// Uses `weight` for line thickness in pixels.
    virtual void
    drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& color,
        f32 weight) = 0;

    /// Draws a thick line with a local transform.
    ///
    /// Uses `matrix` for local transform applied to start/end before drawing.
    /// Uses `start` for start point (local space).
    /// Uses `end` for end point (local space).
    /// Uses `color` for line color.
    /// Uses `weight` for line thickness in pixels.
    virtual void
    drawLine(
        const Matrix3x2& matrix,
        const Vector2& start,
        const Vector2& end,
        const Color& color,
        f32 weight) = 0;

    /// Draws a thick line with separate start/end colors.
    ///
    /// Uses `start` for start point.
    /// Uses `end` for end point.
    /// Uses `startColor` for color at the start point.
    /// Uses `endColor` for color at the end point.
    /// Uses `weight` for line thickness in pixels.
    virtual void
    drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& startColor,
        const Color& endColor,
        f32 weight) = 0;

    /// Draws a filled polyline (series of connected thick line segments).
    ///
    /// Uses `points` for ordered list of vertices.
    /// Uses `thickness` for line thickness in pixels.
    /// Uses `color` for fill color.
    virtual void
    drawPolyline(
        const std::vector<Vector2>& points,
        f32 thickness,
        const Color& color) = 0;

    /// Draws a filled rectangle from a Rect2D.
    ///
    /// Uses `sourceRect` for rectangle to draw.
    /// Uses `color` for fill color.
    virtual void
    drawRectangle(
        const Rect2D& sourceRect,
        const Color& color) = 0;

    /// Draws a filled rectangle with per-corner colors.
    ///
    /// Uses `sourceRect` for rectangle to draw.
    /// Uses `color1` for bottom-left corner color.
    /// Uses `color2` for bottom-right corner color.
    /// Uses `color3` for top-right corner color.
    /// Uses `color4` for top-left corner color.
    virtual void
    drawRectangle(
        const Rect2D& sourceRect,
        const Color& color1,
        const Color& color2,
        const Color& color3,
        const Color& color4) = 0;

    /// Draws a filled rectangle with a local transform.
    ///
    /// Uses `matrix` for local transform.
    /// Uses `position` for bottom-left position.
    /// Uses `width` for width.
    /// Uses `height` for height.
    /// Uses `color` for fill color.
    virtual void
    drawRectangle(
        const Matrix3x2& matrix,
        const Vector2& position,
        f32 width,
        f32 height,
        const Color& color) = 0;

    /// Draws a filled rectangle with a local transform and per-corner colors.
    ///
    /// ```
    /// Y
    /// ▲  color4     color3
    /// │    ┌──────────┐
    /// │    │          │
    /// │    └──────────┘
    /// │  color1     color2
    /// │
    /// └─────────────────► X
    /// ```
    ///
    /// Uses `matrix` for local transform.
    /// Uses `position` for bottom-left position.
    /// Uses `width` for width.
    /// Uses `height` for height.
    /// Uses `color1` for bottom-left corner color.
    /// Uses `color2` for bottom-right corner color.
    /// Uses `color3` for top-right corner color.
    /// Uses `color4` for top-left corner color.
    virtual void
    drawRectangle(
        const Matrix3x2& matrix,
        const Vector2& position,
        f32 width,
        f32 height,
        const Color& color1,
        const Color& color2,
        const Color& color3,
        const Color& color4) = 0;

    /// Draws a filled rectangle with an origin pivot.
    ///
    /// Uses `position` for position of the rectangle.
    /// Uses `width` for width.
    /// Uses `height` for height.
    /// Uses `originPivot` for origin pivot (0..1 per axis).
    /// Uses `color` for fill color.
    virtual void
    drawRectangle(
        const Vector2& position,
        f32 width,
        f32 height,
        const Vector2& originPivot,
        const Color& color) = 0;

    /// Draws a filled sphere approximation (wireframe-like filled segments).
    ///
    /// Uses `position` for center of the sphere.
    /// Uses `radius` for radius.
    /// Uses `color` for fill color.
    /// Uses `segments` for number of segments.
    virtual void
    drawSphere(
        const Vector3& position,
        f32 radius,
        const Color& color,
        i32 segments) = 0;

    /// Draws a filled triangle from three 2D points.
    ///
    /// Uses `point1` for first vertex.
    /// Uses `point2` for second vertex.
    /// Uses `point3` for third vertex.
    /// Uses `color` for fill color.
    virtual void
    drawTriangle(
        const Vector2& point1,
        const Vector2& point2,
        const Vector2& point3,
        const Color& color) = 0;

    /// Draws a filled triangle with per-vertex colors (2D).
    ///
    /// Uses `point1` for first vertex.
    /// Uses `point2` for second vertex.
    /// Uses `point3` for third vertex.
    /// Uses `color1` for color at point1.
    /// Uses `color2` for color at point2.
    /// Uses `color3` for color at point3.
    virtual void
    drawTriangle(
        const Vector2& point1,
        const Vector2& point2,
        const Vector2& point3,
        const Color& color1,
        const Color& color2,
        const Color& color3) = 0;

    /// Draws a filled triangle with per-vertex colors (3D).
    ///
    /// Uses `point1` for first vertex.
    /// Uses `point2` for second vertex.
    /// Uses `point3` for third vertex.
    /// Uses `color1` for color at point1.
    /// Uses `color2` for color at point2.
    /// Uses `color3` for color at point3.
    virtual void
    drawTriangle(
        const Vector3& point1,
        const Vector3& point2,
        const Vector3& point3,
        const Color& color1,
        const Color& color2,
        const Color& color3) = 0;

    /// Flushes the batched primitives, issuing draw commands to the command list.
    ///
    /// This only records GPU draw commands; actual vertex data upload is deferred to submit().
    ///
    /// Uses `commandList` for the command list to record draw commands into.
    /// Uses `primitivePipeline` for the primitive pipeline used for rendering.
    virtual void
    flush(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<PrimitivePipeline>& primitivePipeline) = 0;

    /// Uploads batched vertex data from CPU to GPU.
    ///
    /// Call once per frame after all flush() calls. If the vertex count exceeds
    /// the current buffer capacity, the buffer is resized to fit.
    ///
    /// Uses `graphicsDevice` for the graphics device used to create/resize GPU buffers.
    virtual void
    submit(const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) = 0;

    /// Returns the maximum number of vertices the batch can hold before resizing.
    [[nodiscard]] virtual u32
    getMaxVertexCount() const noexcept = 0;

    /// Returns the number of draw calls issued during the last flush.
    [[nodiscard]] virtual u32
    getDrawCallCount() const noexcept = 0;
};

/// Creates a PrimitivePipeline instance.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<PrimitivePipeline>, std::unique_ptr<Error>>
createPrimitivePipeline(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) noexcept;

/// Creates a PrimitivePipeline instance with custom settings.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<PrimitivePipeline>, std::unique_ptr<Error>>
createPrimitivePipeline(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::DepthStencilDesc>&& depthStencilDesc,
    std::optional<gpu::RasterizerDesc>&& rasterizerDesc) noexcept;

/// Creates a PrimitiveBatch instance (batch buffers only).
///
/// Uses `graphicsDevice` for the graphics device.
/// Uses `batchSize` for optional initial batch size for the vertex buffer (default: 4096).
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<PrimitiveBatch>, std::unique_ptr<Error>>
createPrimitiveBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize = std::nullopt) noexcept;

} // namespace pomdog
