// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class AssetManager;
class BoundingBox;
class Color;
class Matrix3x2;
class Matrix4x4;
class Rectangle;
class Vector2;
class Vector3;
template <typename T>
class Radian;
} // namespace pomdog

namespace pomdog {

class POMDOG_EXPORT PolylineBatch final {
public:
    PolylineBatch(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        AssetManager& assets);

    ~PolylineBatch();

    void begin(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const Matrix4x4& transformMatrix);

    void drawPath(
        const std::vector<Vector2>& path,
        bool closed,
        const Color& color,
        f32 thickness);

    // void drawArc(
    //    const Vector2& position,
    //    f32 radius,
    //    const Radian<f32>& startAngle,
    //    const Radian<f32>& arcAngle,
    //    const Color& color);

    void drawBox(
        const BoundingBox& box,
        const Color& color,
        f32 thickness);

    void drawBox(
        const Vector3& position,
        const Vector3& scale,
        const Color& color,
        f32 thickness);

    void drawBox(
        const Vector3& position,
        const Vector3& scale,
        const Vector3& originPivot,
        const Color& color,
        f32 thickness);

    void drawCircle(
        const Vector2& position,
        f32 radius,
        const Color& color,
        i32 segments,
        f32 thickness);

    // void drawEllipse();

    void drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& color,
        f32 thickness);

    void drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& startColor,
        const Color& endColor,
        f32 thickness);

    void drawLine(
        const Vector3& start,
        const Vector3& end,
        const Color& color,
        f32 thickness);

    void drawLine(
        const Vector3& start,
        const Vector3& end,
        const Color& startColor,
        const Color& endColor,
        f32 thickness);

    void drawRectangle(
        const Rectangle& sourceRect,
        const Color& color,
        f32 thickness);

    void drawRectangle(
        const Rectangle& sourceRect,
        const Color& color1,
        const Color& color2,
        const Color& color3,
        const Color& color4,
        f32 thickness);

    void drawRectangle(
        const Matrix3x2& matrix,
        const Rectangle& sourceRect,
        const Color& color,
        f32 thickness);

    void drawSphere(
        const Vector3& position,
        f32 radius,
        const Color& color,
        i32 segments,
        f32 thickness);

    void drawTriangle(
        const Vector2& point1,
        const Vector2& point2,
        const Vector2& point3,
        const Color& color,
        f32 thickness);

    void drawTriangle(
        const Vector2& point1,
        const Vector2& point2,
        const Vector2& point3,
        const Color& color1,
        const Color& color2,
        const Color& color3,
        f32 thickness);

    void end();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog
