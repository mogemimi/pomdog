// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_LINEBATCH_EA425637_HPP
#define POMDOG_LINEBATCH_EA425637_HPP

#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class LineBatch {
public:
    LineBatch(
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets);

    ~LineBatch();

    void Begin(
        std::shared_ptr<GraphicsCommandList> const& commandList,
        Matrix4x4 const& transformMatrix);

    //void DrawArc(
    //    Vector2 const& position,
    //    float radius,
    //    Radian<float> const& startAngle,
    //    Radian<float> const& arcAngle,
    //    Color const& color);

    void DrawCircle(
        Vector2 const& position,
        float radius,
        Color const& color,
        std::size_t segments);

    //void DrawEllipse();

    void DrawLine(
        Vector2 const& start,
        Vector2 const& end,
        Color const& color);

    void DrawLine(
        Vector2 const& start,
        Vector2 const& end,
        Color const& startColor,
        Color const& endColor);

    void DrawLine(
        Vector3 const& start,
        Vector3 const& end,
        Color const& color);

    void DrawLine(
        Vector3 const& start,
        Vector3 const& end,
        Color const& startColor,
        Color const& endColor);

    void DrawRectangle(
        Rectangle const& sourceRect,
        Color const& color);

    void DrawRectangle(
        Rectangle const& sourceRect,
        Color const& color1,
        Color const& color2,
        Color const& color3,
        Color const& color4);

    void DrawRectangle(
        Matrix3x2 const& matrix,
        Rectangle const& sourceRect,
        Color const& color);

    void DrawTriangle(
        Vector2 const& point1,
        Vector2 const& point2,
        Vector2 const& point3,
        Color const& color);

    void DrawTriangle(
        Vector2 const& point1,
        Vector2 const& point2,
        Vector2 const& point3,
        Color const& color1,
        Color const& color2,
        Color const& color3);

    void End();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog

#endif // POMDOG_LINEBATCH_EA425637_HPP
