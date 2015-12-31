// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <vector>
#include <memory>

namespace Pomdog {

class PolygonBatch {
public:
    PolygonBatch(
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets);

    ~PolygonBatch();

    void Begin(
        std::shared_ptr<GraphicsCommandList> const& commandList,
        Matrix4x4 const& transformMatrix);

    void DrawArc(
        Vector2 const& position,
        float radius,
        Radian<float> const& startAngle,
        Radian<float> const& arcAngle,
        std::size_t segments,
        Color const& color);

    void DrawBox(
        BoundingBox const& box,
        Color const& color);

    void DrawBox(
        Vector3 const& position,
        Vector3 const& scale,
        Color const& color);

    void DrawBox(
        Vector3 const& position,
        Vector3 const& scale,
        Vector3 const& originPivot,
        Color const& color);

    void DrawCircle(
        Vector2 const& position,
        float radius,
        std::size_t segments,
        Color const& color);

    void DrawCircle(
        Vector3 const& position,
        float radius,
        std::size_t segments,
        Color const& color);

    //void DrawEllipse();

    void DrawLine(
        Vector2 const& start,
        Vector2 const& end,
        Color const& color,
        float weight);

    void DrawLine(
        Vector2 const& start,
        Vector2 const& end,
        Color const& startColor,
        Color const& endColor,
        float weight);

    void DrawPolyline(
        std::vector<Vector2> const& points,
        float thickness,
        Color const& color);

    //void DrawPolygon(
    //    std::vector<PolygonBatchVertex> const& vertices,
    //    Color const& color);

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
    void DrawRectangle(
        Matrix3x2 const& matrix,
        Rectangle const& sourceRect,
        Color const& color1,
        Color const& color2,
        Color const& color3,
        Color const& color4);

    void DrawRectangle(
        Vector2 const& position,
        float width,
        float height,
        Vector2 const& originPivot,
        Color const& color);

    void DrawSphere(
        Vector3 const& position,
        float radius,
        Color const& color,
        std::size_t segments);

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

    void DrawTriangle(
        Vector3 const& point1,
        Vector3 const& point2,
        Vector3 const& point3,
        Color const& color1,
        Color const& color2,
        Color const& color3);

    void DrawTriangle(
        Vector3 const& point1,
        Vector3 const& point2,
        Vector3 const& point3,
        Vector4 const& color1,
        Vector4 const& color2,
        Vector4 const& color3);

    void End();

    std::size_t GetMaxVertexCount() const noexcept;

    std::uint32_t GetDrawCallCount() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
