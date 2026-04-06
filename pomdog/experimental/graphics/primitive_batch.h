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
    /// @param transformMatrix The view-projection matrix.
    virtual void
    setTransform(const Matrix4x4& transformMatrix) = 0;

    /// Draws a filled arc (pie shape).
    ///
    /// @param position Center of the arc.
    /// @param radius Radius of the arc.
    /// @param startAngle Starting angle in radians.
    /// @param arcAngle Sweep angle in radians.
    /// @param segments Number of triangle segments used to approximate the arc.
    /// @param color Fill color.
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
    /// @param box The axis-aligned bounding box.
    /// @param color Fill color.
    virtual void
    drawBox(
        const BoundingBox& box,
        const Color& color) = 0;

    /// Draws a filled box at the given position and scale.
    ///
    /// @param position Position of the box origin.
    /// @param scale Size of the box along each axis.
    /// @param color Fill color.
    virtual void
    drawBox(
        const Vector3& position,
        const Vector3& scale,
        const Color& color) = 0;

    /// Draws a filled box at the given position, scale, and origin pivot.
    ///
    /// @param position Position of the box.
    /// @param scale Size of the box along each axis.
    /// @param originPivot Origin pivot point (0..1 per axis).
    /// @param color Fill color.
    virtual void
    drawBox(
        const Vector3& position,
        const Vector3& scale,
        const Vector3& originPivot,
        const Color& color) = 0;

    /// Draws a filled circle in 2D.
    ///
    /// @param position Center of the circle.
    /// @param radius Radius.
    /// @param segments Number of triangle segments.
    /// @param color Fill color.
    virtual void
    drawCircle(
        const Vector2& position,
        f32 radius,
        i32 segments,
        const Color& color) = 0;

    /// Draws a filled circle in 3D.
    ///
    /// @param position Center of the circle.
    /// @param radius Radius.
    /// @param segments Number of triangle segments.
    /// @param color Fill color.
    virtual void
    drawCircle(
        const Vector3& position,
        f32 radius,
        i32 segments,
        const Color& color) = 0;

    /// Draws a thick line between two 2D points.
    ///
    /// @param start Start point.
    /// @param end End point.
    /// @param color Line color.
    /// @param weight Line thickness in pixels.
    virtual void
    drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& color,
        f32 weight) = 0;

    /// Draws a thick line with a local transform.
    ///
    /// @param matrix Local transform applied to start/end before drawing.
    /// @param start Start point (local space).
    /// @param end End point (local space).
    /// @param color Line color.
    /// @param weight Line thickness in pixels.
    virtual void
    drawLine(
        const Matrix3x2& matrix,
        const Vector2& start,
        const Vector2& end,
        const Color& color,
        f32 weight) = 0;

    /// Draws a thick line with separate start/end colors.
    ///
    /// @param start Start point.
    /// @param end End point.
    /// @param startColor Color at the start point.
    /// @param endColor Color at the end point.
    /// @param weight Line thickness in pixels.
    virtual void
    drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& startColor,
        const Color& endColor,
        f32 weight) = 0;

    /// Draws a filled polyline (series of connected thick line segments).
    ///
    /// @param points Ordered list of vertices.
    /// @param thickness Line thickness in pixels.
    /// @param color Fill color.
    virtual void
    drawPolyline(
        const std::vector<Vector2>& points,
        f32 thickness,
        const Color& color) = 0;

    /// Draws a filled rectangle from a Rect2D.
    ///
    /// @param sourceRect Rectangle to draw.
    /// @param color Fill color.
    virtual void
    drawRectangle(
        const Rect2D& sourceRect,
        const Color& color) = 0;

    /// Draws a filled rectangle with per-corner colors.
    ///
    /// @param sourceRect Rectangle to draw.
    /// @param color1 Bottom-left corner color.
    /// @param color2 Bottom-right corner color.
    /// @param color3 Top-right corner color.
    /// @param color4 Top-left corner color.
    virtual void
    drawRectangle(
        const Rect2D& sourceRect,
        const Color& color1,
        const Color& color2,
        const Color& color3,
        const Color& color4) = 0;

    /// Draws a filled rectangle with a local transform.
    ///
    /// @param matrix Local transform.
    /// @param position Bottom-left position.
    /// @param width Width.
    /// @param height Height.
    /// @param color Fill color.
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
    /// @param matrix Local transform.
    /// @param position Bottom-left position.
    /// @param width Width.
    /// @param height Height.
    /// @param color1 Bottom-left corner color.
    /// @param color2 Bottom-right corner color.
    /// @param color3 Top-right corner color.
    /// @param color4 Top-left corner color.
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
    /// @param position Position of the rectangle.
    /// @param width Width.
    /// @param height Height.
    /// @param originPivot Origin pivot (0..1 per axis).
    /// @param color Fill color.
    virtual void
    drawRectangle(
        const Vector2& position,
        f32 width,
        f32 height,
        const Vector2& originPivot,
        const Color& color) = 0;

    /// Draws a filled sphere approximation (wireframe-like filled segments).
    ///
    /// @param position Center of the sphere.
    /// @param radius Radius.
    /// @param color Fill color.
    /// @param segments Number of segments.
    virtual void
    drawSphere(
        const Vector3& position,
        f32 radius,
        const Color& color,
        i32 segments) = 0;

    /// Draws a filled triangle from three 2D points.
    ///
    /// @param point1 First vertex.
    /// @param point2 Second vertex.
    /// @param point3 Third vertex.
    /// @param color Fill color.
    virtual void
    drawTriangle(
        const Vector2& point1,
        const Vector2& point2,
        const Vector2& point3,
        const Color& color) = 0;

    /// Draws a filled triangle with per-vertex colors (2D).
    ///
    /// @param point1 First vertex.
    /// @param point2 Second vertex.
    /// @param point3 Third vertex.
    /// @param color1 Color at point1.
    /// @param color2 Color at point2.
    /// @param color3 Color at point3.
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
    /// @param point1 First vertex.
    /// @param point2 Second vertex.
    /// @param point3 Third vertex.
    /// @param color1 Color at point1.
    /// @param color2 Color at point2.
    /// @param color3 Color at point3.
    virtual void
    drawTriangle(
        const Vector3& point1,
        const Vector3& point2,
        const Vector3& point3,
        const Color& color1,
        const Color& color2,
        const Color& color3) = 0;

    /// Draws a filled triangle with per-vertex Vector4 colors (3D).
    ///
    /// @param point1 First vertex.
    /// @param point2 Second vertex.
    /// @param point3 Third vertex.
    /// @param color1 Color at point1 (RGBA as Vector4).
    /// @param color2 Color at point2 (RGBA as Vector4).
    /// @param color3 Color at point3 (RGBA as Vector4).
    virtual void
    drawTriangle(
        const Vector3& point1,
        const Vector3& point2,
        const Vector3& point3,
        const Vector4& color1,
        const Vector4& color2,
        const Vector4& color3) = 0;

    /// Flushes the batched primitives, issuing draw commands to the command list.
    ///
    /// This only records GPU draw commands; actual vertex data upload is deferred to submit().
    ///
    /// @param commandList The command list to record draw commands into.
    /// @param primitivePipeline The primitive pipeline used for rendering.
    virtual void
    flush(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<PrimitivePipeline>& primitivePipeline) = 0;

    /// Uploads batched vertex data from CPU to GPU.
    ///
    /// Call once per frame after all flush() calls. If the vertex count exceeds
    /// the current buffer capacity, the buffer is resized to fit.
    ///
    /// @param graphicsDevice The graphics device used to create/resize GPU buffers.
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
/// @param graphicsDevice The graphics device.
/// @param batchSize Optional initial batch size for the vertex buffer (default: 4096).
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<PrimitiveBatch>, std::unique_ptr<Error>>
createPrimitiveBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize = std::nullopt) noexcept;

} // namespace pomdog
