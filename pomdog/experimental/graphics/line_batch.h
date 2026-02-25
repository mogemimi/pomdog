// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class AssetManager;
class BoundingBox;
class Color;
class Matrix3x2;
class Matrix4x4;
class Rect2D;
class Vector2;
class Vector3;
template <typename T>
class Radian;
} // namespace pomdog

namespace pomdog {

class POMDOG_EXPORT LineBatch final {
public:
    LineBatch(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        AssetManager& assets);

    ~LineBatch();

    void begin(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const Matrix4x4& transformMatrix);

    // void drawArc(
    //    const Vector2& position,
    //    f32 radius,
    //    const Radian<f32>& startAngle,
    //    const Radian<f32>& arcAngle,
    //    const Color& color);

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
        const Color& color,
        i32 segments);

    // void drawEllipse();

    void drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& color);

    void drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& startColor,
        const Color& endColor);

    void drawLine(
        const Vector3& start,
        const Vector3& end,
        const Color& color);

    void drawLine(
        const Vector3& start,
        const Vector3& end,
        const Color& startColor,
        const Color& endColor);

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
        const Rect2D& sourceRect,
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

    void end();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog
