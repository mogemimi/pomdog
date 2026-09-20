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
    /// Uses `transformMatrix` for the view-projection matrix.
    virtual void
    setTransform(const Matrix4x4& transformMatrix) = 0;

    /// Draws a wireframe box from a bounding box.
    ///
    /// Uses `box` for the axis-aligned bounding box.
    /// Uses `color` for line color.
    virtual void
    drawBox(
        const BoundingBox& box,
        const Color& color) = 0;

    /// Draws a wireframe box at the given position and scale.
    ///
    /// Uses `position` for position of the box origin.
    /// Uses `scale` for size of the box along each axis.
    /// Uses `color` for line color.
    virtual void
    drawBox(
        const Vector3& position,
        const Vector3& scale,
        const Color& color) = 0;

    /// Draws a wireframe box at the given position, scale, and origin pivot.
    ///
    /// Uses `position` for position of the box.
    /// Uses `scale` for size of the box along each axis.
    /// Uses `originPivot` for origin pivot point (0..1 per axis).
    /// Uses `color` for line color.
    virtual void
    drawBox(
        const Vector3& position,
        const Vector3& scale,
        const Vector3& originPivot,
        const Color& color) = 0;

    /// Draws a wireframe circle.
    ///
    /// Uses `position` for center of the circle.
    /// Uses `radius` for radius.
    /// Uses `color` for line color.
    /// Uses `segments` for number of line segments.
    virtual void
    drawCircle(
        const Vector2& position,
        f32 radius,
        const Color& color,
        i32 segments) = 0;

    /// Draws a line between two 2D points.
    ///
    /// Uses `start` for start point.
    /// Uses `end` for end point.
    /// Uses `color` for line color.
    virtual void
    drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& color) = 0;

    /// Draws a line with separate start/end colors (2D).
    ///
    /// Uses `start` for start point.
    /// Uses `end` for end point.
    /// Uses `startColor` for color at the start point.
    /// Uses `endColor` for color at the end point.
    virtual void
    drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& startColor,
        const Color& endColor) = 0;

    /// Draws a line between two 3D points.
    ///
    /// Uses `start` for start point.
    /// Uses `end` for end point.
    /// Uses `color` for line color.
    virtual void
    drawLine(
        const Vector3& start,
        const Vector3& end,
        const Color& color) = 0;

    /// Draws a line with separate start/end colors (3D).
    ///
    /// Uses `start` for start point.
    /// Uses `end` for end point.
    /// Uses `startColor` for color at the start point.
    /// Uses `endColor` for color at the end point.
    virtual void
    drawLine(
        const Vector3& start,
        const Vector3& end,
        const Color& startColor,
        const Color& endColor) = 0;

    /// Draws a wireframe rectangle from a Rect2D.
    ///
    /// Uses `sourceRect` for rectangle to draw.
    /// Uses `color` for line color.
    virtual void
    drawRectangle(
        const Rect2D& sourceRect,
        const Color& color) = 0;

    /// Draws a wireframe rectangle with per-corner colors.
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

    /// Draws a wireframe rectangle with a local transform.
    ///
    /// Uses `matrix` for local transform.
    /// Uses `sourceRect` for rectangle to draw.
    /// Uses `color` for line color.
    virtual void
    drawRectangle(
        const Matrix3x2& matrix,
        const Rect2D& sourceRect,
        const Color& color) = 0;

    /// Draws a wireframe sphere approximation.
    ///
    /// Uses `position` for center of the sphere.
    /// Uses `radius` for radius.
    /// Uses `color` for line color.
    /// Uses `segments` for number of segments.
    virtual void
    drawSphere(
        const Vector3& position,
        f32 radius,
        const Color& color,
        i32 segments) = 0;

    /// Draws a wireframe triangle from three 2D points.
    ///
    /// Uses `point1` for first vertex.
    /// Uses `point2` for second vertex.
    /// Uses `point3` for third vertex.
    /// Uses `color` for line color.
    virtual void
    drawTriangle(
        const Vector2& point1,
        const Vector2& point2,
        const Vector2& point3,
        const Color& color) = 0;

    /// Draws a wireframe triangle with per-vertex colors.
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

    /// Flushes the batched lines, issuing draw commands to the command list.
    ///
    /// This only records GPU draw commands; actual vertex data upload is deferred to submit().
    ///
    /// Uses `commandList` for the command list to record draw commands into.
    /// Uses `linePipeline` for the line pipeline used for rendering.
    virtual void
    flush(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<LinePipeline>& linePipeline) = 0;

    /// Uploads batched vertex data from CPU to GPU.
    ///
    /// Call once per frame after all flush() calls. If the vertex count exceeds
    /// the current buffer capacity, the buffer is resized to fit.
    ///
    /// Uses `graphicsDevice` for the graphics device used to create/resize GPU buffers.
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
/// Uses `graphicsDevice` for the graphics device.
/// Uses `batchSize` for optional initial batch size for the vertex buffer (default: 4096).
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<LineBatch>, std::unique_ptr<Error>>
createLineBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize = std::nullopt) noexcept;

} // namespace pomdog
