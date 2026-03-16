// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class CommandList;
class GraphicsDevice;
struct DepthStencilDescriptor;
struct RasterizerDescriptor;
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

/// Renders filled primitive shapes (triangles, circles, rectangles, etc.) in a scene.
class POMDOG_EXPORT PrimitiveBatch final {
public:
    PrimitiveBatch();

    PrimitiveBatch(const PrimitiveBatch&) = delete;
    PrimitiveBatch& operator=(const PrimitiveBatch&) = delete;
    PrimitiveBatch(PrimitiveBatch&&) = default;
    PrimitiveBatch& operator=(PrimitiveBatch&&) = default;

    ~PrimitiveBatch();

    /// Initializes the primitive batch by loading shaders and creating GPU resources.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    /// Initializes the primitive batch with custom depth-stencil and rasterizer settings.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<gpu::DepthStencilDescriptor>&& depthStencilDesc,
        std::optional<gpu::RasterizerDescriptor>&& rasterizerDesc);

    void begin(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const Matrix4x4& transformMatrix);

    void drawArc(
        const Vector2& position,
        f32 radius,
        const Radian<f32>& startAngle,
        const Radian<f32>& arcAngle,
        i32 segments,
        const Color& color);

    void drawBox(
        const BoundingBox& box,
        const Color& color);

    void drawBox(
        const Vector3& position,
        const Vector3& scale,
        const Color& color);

    void drawBox(
        const Vector3& position,
        const Vector3& scale,
        const Vector3& originPivot,
        const Color& color);

    void drawCircle(
        const Vector2& position,
        f32 radius,
        i32 segments,
        const Color& color);

    void drawCircle(
        const Vector3& position,
        f32 radius,
        i32 segments,
        const Color& color);

    // void drawEllipse();

    void drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& color,
        f32 weight);

    void drawLine(
        const Matrix3x2& matrix,
        const Vector2& start,
        const Vector2& end,
        const Color& color,
        f32 weight);

    void drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& startColor,
        const Color& endColor,
        f32 weight);

    void drawPolyline(
        const std::vector<Vector2>& points,
        f32 thickness,
        const Color& color);

    // void drawPolygon(
    //    const std::vector<PrimitiveBatchVertex>& vertices,
    //    const Color& color);

    void drawRectangle(
        const Rect2D& sourceRect,
        const Color& color);

    void drawRectangle(
        const Rect2D& sourceRect,
        const Color& color1,
        const Color& color2,
        const Color& color3,
        const Color& color4);

    void drawRectangle(
        const Matrix3x2& matrix,
        const Vector2& position,
        f32 width,
        f32 height,
        const Color& color);

    ///@note
    /// Y
    /// ^   color4      color3
    /// |    +----------+
    /// |    |          |
    /// |    +----------+
    /// |   color1      color2
    /// |
    /// +-----------------> X
    ///
    void drawRectangle(
        const Matrix3x2& matrix,
        const Vector2& position,
        f32 width,
        f32 height,
        const Color& color1,
        const Color& color2,
        const Color& color3,
        const Color& color4);

    void drawRectangle(
        const Vector2& position,
        f32 width,
        f32 height,
        const Vector2& originPivot,
        const Color& color);

    void drawSphere(
        const Vector3& position,
        f32 radius,
        const Color& color,
        i32 segments);

    void drawTriangle(
        const Vector2& point1,
        const Vector2& point2,
        const Vector2& point3,
        const Color& color);

    void drawTriangle(
        const Vector2& point1,
        const Vector2& point2,
        const Vector2& point3,
        const Color& color1,
        const Color& color2,
        const Color& color3);

    void drawTriangle(
        const Vector3& point1,
        const Vector3& point2,
        const Vector3& point3,
        const Color& color1,
        const Color& color2,
        const Color& color3);

    void drawTriangle(
        const Vector3& point1,
        const Vector3& point2,
        const Vector3& point3,
        const Vector4& color1,
        const Vector4& color2,
        const Vector4& color3);

    void flush();

    void end();

    [[nodiscard]] u32
    getMaxVertexCount() const noexcept;

    [[nodiscard]] u32
    getDrawCallCount() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog
