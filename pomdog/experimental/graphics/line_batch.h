// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class CommandList;
class GraphicsDevice;
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
template <typename T>
class Radian;
} // namespace pomdog

namespace pomdog {

/// Holds the pipeline state (shaders, blend state, etc.) for LineBatch.
class POMDOG_EXPORT LinePipeline {
public:
    virtual ~LinePipeline();
};

/// Renders line primitives in a scene.
///
/// Usage pattern:
/// ```cpp
/// lineBatch->reset();
/// lineBatch->setTransform(projectionMatrix);
/// lineBatch->drawLine(start, end, color);
/// lineBatch->flush(commandList, linePipeline);
/// lineBatch->submit(graphicsDevice);
/// ```
class POMDOG_EXPORT LineBatch {
public:
    virtual ~LineBatch();

    /// Resets the batch state, clearing any previously batched vertices.
    virtual void
    reset() = 0;

    /// Sets the transformation matrix to be applied to all lines in the batch.
    ///
    /// @param transformMatrix The view-projection matrix.
    virtual void
    setTransform(const Matrix4x4& transformMatrix) = 0;

    /// Draws a wireframe box from a bounding box.
    ///
    /// @param box The axis-aligned bounding box.
    /// @param color Line color.
    virtual void
    drawBox(
        const BoundingBox& box,
        const Color& color) = 0;

    /// Draws a wireframe box at the given position and scale.
    ///
    /// @param position Position of the box origin.
    /// @param scale Size of the box along each axis.
    /// @param color Line color.
    virtual void
    drawBox(
        const Vector3& position,
        const Vector3& scale,
        const Color& color) = 0;

    /// Draws a wireframe box at the given position, scale, and origin pivot.
    ///
    /// @param position Position of the box.
    /// @param scale Size of the box along each axis.
    /// @param originPivot Origin pivot point (0..1 per axis).
    /// @param color Line color.
    virtual void
    drawBox(
        const Vector3& position,
        const Vector3& scale,
        const Vector3& originPivot,
        const Color& color) = 0;

    /// Draws a wireframe circle.
    ///
    /// @param position Center of the circle.
    /// @param radius Radius.
    /// @param color Line color.
    /// @param segments Number of line segments.
    virtual void
    drawCircle(
        const Vector2& position,
        f32 radius,
        const Color& color,
        i32 segments) = 0;

    /// Draws a line between two 2D points.
    ///
    /// @param start Start point.
    /// @param end End point.
    /// @param color Line color.
    virtual void
    drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& color) = 0;

    /// Draws a line with separate start/end colors (2D).
    ///
    /// @param start Start point.
    /// @param end End point.
    /// @param startColor Color at the start point.
    /// @param endColor Color at the end point.
    virtual void
    drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& startColor,
        const Color& endColor) = 0;

    /// Draws a line between two 3D points.
    ///
    /// @param start Start point.
    /// @param end End point.
    /// @param color Line color.
    virtual void
    drawLine(
        const Vector3& start,
        const Vector3& end,
        const Color& color) = 0;

    /// Draws a line with separate start/end colors (3D).
    ///
    /// @param start Start point.
    /// @param end End point.
    /// @param startColor Color at the start point.
    /// @param endColor Color at the end point.
    virtual void
    drawLine(
        const Vector3& start,
        const Vector3& end,
        const Color& startColor,
        const Color& endColor) = 0;

    /// Draws a wireframe rectangle from a Rect2D.
    ///
    /// @param sourceRect Rectangle to draw.
    /// @param color Line color.
    virtual void
    drawRectangle(
        const Rect2D& sourceRect,
        const Color& color) = 0;

    /// Draws a wireframe rectangle with per-corner colors.
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

    /// Draws a wireframe rectangle with a local transform.
    ///
    /// @param matrix Local transform.
    /// @param sourceRect Rectangle to draw.
    /// @param color Line color.
    virtual void
    drawRectangle(
        const Matrix3x2& matrix,
        const Rect2D& sourceRect,
        const Color& color) = 0;

    /// Draws a wireframe sphere approximation.
    ///
    /// @param position Center of the sphere.
    /// @param radius Radius.
    /// @param color Line color.
    /// @param segments Number of segments.
    virtual void
    drawSphere(
        const Vector3& position,
        f32 radius,
        const Color& color,
        i32 segments) = 0;

    /// Draws a wireframe triangle from three 2D points.
    ///
    /// @param point1 First vertex.
    /// @param point2 Second vertex.
    /// @param point3 Third vertex.
    /// @param color Line color.
    virtual void
    drawTriangle(
        const Vector2& point1,
        const Vector2& point2,
        const Vector2& point3,
        const Color& color) = 0;

    /// Draws a wireframe triangle with per-vertex colors.
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

    /// Flushes the batched lines, issuing draw commands to the command list.
    ///
    /// This only records GPU draw commands; actual vertex data upload is deferred to submit().
    ///
    /// @param commandList The command list to record draw commands into.
    /// @param linePipeline The line pipeline used for rendering.
    virtual void
    flush(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<LinePipeline>& linePipeline) = 0;

    /// Uploads batched vertex data from CPU to GPU.
    ///
    /// Call once per frame after all flush() calls. If the vertex count exceeds
    /// the current buffer capacity, the buffer is resized to fit.
    ///
    /// @param graphicsDevice The graphics device used to create/resize GPU buffers.
    virtual void
    submit(const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) = 0;

    /// Returns the number of draw calls issued during the last flush.
    [[nodiscard]] virtual u32
    getDrawCallCount() const noexcept = 0;
};

/// Creates a LinePipeline instance.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<LinePipeline>, std::unique_ptr<Error>>
createLinePipeline(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) noexcept;

/// Creates a LineBatch instance (batch buffers only).
///
/// @param graphicsDevice The graphics device.
/// @param batchSize Optional initial batch size for the vertex buffer (default: 4096).
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<LineBatch>, std::unique_ptr<Error>>
createLineBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize = std::nullopt) noexcept;

} // namespace pomdog
