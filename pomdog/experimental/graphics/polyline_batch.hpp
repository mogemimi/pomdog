// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/math/forward_declarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class AssetManager;

class POMDOG_EXPORT PolylineBatch final {
public:
    PolylineBatch(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager& assets);

    ~PolylineBatch();

    void Begin(
        const std::shared_ptr<GraphicsCommandList>& commandList,
        const Matrix4x4& transformMatrix);

    void DrawPath(
        const std::vector<Vector2>& path,
        bool closed,
        const Color& color,
        float thickness);

    //void DrawArc(
    //    const Vector2& position,
    //    float radius,
    //    const Radian<float>& startAngle,
    //    const Radian<float>& arcAngle,
    //    const Color& color);

    void DrawBox(
        const BoundingBox& box,
        const Color& color,
        float thickness);

    void DrawBox(
        const Vector3& position,
        const Vector3& scale,
        const Color& color,
        float thickness);

    void DrawBox(
        const Vector3& position,
        const Vector3& scale,
        const Vector3& originPivot,
        const Color& color,
        float thickness);

    void DrawCircle(
        const Vector2& position,
        float radius,
        const Color& color,
        int segments,
        float thickness);

    //void DrawEllipse();

    void DrawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& color,
        float thickness);

    void DrawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& startColor,
        const Color& endColor,
        float thickness);

    void DrawLine(
        const Vector3& start,
        const Vector3& end,
        const Color& color,
        float thickness);

    void DrawLine(
        const Vector3& start,
        const Vector3& end,
        const Color& startColor,
        const Color& endColor,
        float thickness);

    void DrawRectangle(
        const Rectangle& sourceRect,
        const Color& color,
        float thickness);

    void DrawRectangle(
        const Rectangle& sourceRect,
        const Color& color1,
        const Color& color2,
        const Color& color3,
        const Color& color4,
        float thickness);

    void DrawRectangle(
        const Matrix3x2& matrix,
        const Rectangle& sourceRect,
        const Color& color,
        float thickness);

    void DrawSphere(
        const Vector3& position,
        float radius,
        const Color& color,
        int segments,
        float thickness);

    void DrawTriangle(
        const Vector2& point1,
        const Vector2& point2,
        const Vector2& point3,
        const Color& color,
        float thickness);

    void DrawTriangle(
        const Vector2& point1,
        const Vector2& point2,
        const Vector2& point3,
        const Color& color1,
        const Color& color2,
        const Color& color3,
        float thickness);

    void End();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
