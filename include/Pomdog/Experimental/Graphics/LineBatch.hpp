// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class AssetManager;

class POMDOG_EXPORT LineBatch final {
public:
    LineBatch(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager& assets);

    ~LineBatch();

    void Begin(
        const std::shared_ptr<GraphicsCommandList>& commandList,
        const Matrix4x4& transformMatrix);

    //void DrawArc(
    //    const Vector2& position,
    //    float radius,
    //    const Radian<float>& startAngle,
    //    const Radian<float>& arcAngle,
    //    const Color& color);

    void DrawBox(
        const BoundingBox& box,
        const Color& color);

    void DrawBox(
        const Vector3& position,
        const Vector3& scale,
        const Color& color);

    void DrawBox(
        const Vector3& position,
        const Vector3& scale,
        const Vector3& originPivot,
        const Color& color);

    void DrawCircle(
        const Vector2& position,
        float radius,
        const Color& color,
        int segments);

    //void DrawEllipse();

    void DrawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& color);

    void DrawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& startColor,
        const Color& endColor);

    void DrawLine(
        const Vector3& start,
        const Vector3& end,
        const Color& color);

    void DrawLine(
        const Vector3& start,
        const Vector3& end,
        const Color& startColor,
        const Color& endColor);

    void DrawRectangle(
        const Rectangle& sourceRect,
        const Color& color);

    void DrawRectangle(
        const Rectangle& sourceRect,
        const Color& color1,
        const Color& color2,
        const Color& color3,
        const Color& color4);

    void DrawRectangle(
        const Matrix3x2& matrix,
        const Rectangle& sourceRect,
        const Color& color);

    void DrawSphere(
        const Vector3& position,
        float radius,
        const Color& color,
        int segments);

    void DrawTriangle(
        const Vector2& point1,
        const Vector2& point2,
        const Vector2& point3,
        const Color& color);

    void DrawTriangle(
        const Vector2& point1,
        const Vector2& point2,
        const Vector2& point3,
        const Color& color1,
        const Color& color2,
        const Color& color3);

    void End();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
