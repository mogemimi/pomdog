// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <tuple>
#include <vector>
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

/// Renders polyline primitives with configurable thickness in a scene.
class POMDOG_EXPORT PolylineBatch {
public:
    virtual ~PolylineBatch();

    virtual void
    begin(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const Matrix4x4& transformMatrix) = 0;

    virtual void
    drawPath(
        const std::vector<Vector2>& path,
        bool closed,
        const Color& color,
        f32 thickness) = 0;

    virtual void
    drawBox(
        const BoundingBox& box,
        const Color& color,
        f32 thickness) = 0;

    virtual void
    drawBox(
        const Vector3& position,
        const Vector3& scale,
        const Color& color,
        f32 thickness) = 0;

    virtual void
    drawBox(
        const Vector3& position,
        const Vector3& scale,
        const Vector3& originPivot,
        const Color& color,
        f32 thickness) = 0;

    virtual void
    drawCircle(
        const Vector2& position,
        f32 radius,
        const Color& color,
        i32 segments,
        f32 thickness) = 0;

    virtual void
    drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& color,
        f32 thickness) = 0;

    virtual void
    drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& startColor,
        const Color& endColor,
        f32 thickness) = 0;

    virtual void
    drawLine(
        const Vector3& start,
        const Vector3& end,
        const Color& color,
        f32 thickness) = 0;

    virtual void
    drawLine(
        const Vector3& start,
        const Vector3& end,
        const Color& startColor,
        const Color& endColor,
        f32 thickness) = 0;

    virtual void
    drawRectangle(
        const Rect2D& sourceRect,
        const Color& color,
        f32 thickness) = 0;

    virtual void
    drawRectangle(
        const Rect2D& sourceRect,
        const Color& color1,
        const Color& color2,
        const Color& color3,
        const Color& color4,
        f32 thickness) = 0;

    virtual void
    drawRectangle(
        const Matrix3x2& matrix,
        const Rect2D& sourceRect,
        const Color& color,
        f32 thickness) = 0;

    virtual void
    drawSphere(
        const Vector3& position,
        f32 radius,
        const Color& color,
        i32 segments,
        f32 thickness) = 0;

    virtual void
    drawTriangle(
        const Vector2& point1,
        const Vector2& point2,
        const Vector2& point3,
        const Color& color,
        f32 thickness) = 0;

    virtual void
    drawTriangle(
        const Vector2& point1,
        const Vector2& point2,
        const Vector2& point3,
        const Color& color1,
        const Color& color2,
        const Color& color3,
        f32 thickness) = 0;

    virtual void
    end() = 0;
};

[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<PolylineBatch>, std::unique_ptr<Error>>
createPolylineBatch(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) noexcept;

} // namespace pomdog
