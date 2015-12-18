// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_POLYGONSHAPEBUILDER_E6B8BC2B_HPP
#define POMDOG_POLYGONSHAPEBUILDER_E6B8BC2B_HPP

#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Math/Radian.hpp"
#include <cstdint>
#include <functional>
#include <vector>

namespace Pomdog {

struct PolygonBatchVertex {
    // {xyz} = position.xyz
    Vector3 Position;

    // {xyzw} = color.rgba
    Vector4 Color;
};

class PolygonShapeBuilder {
private:
    static constexpr std::size_t MaxVertexCount = 4096;
    static constexpr std::size_t MinVertexCount = 256;

    typedef PolygonBatchVertex Vertex;
    std::vector<PolygonBatchVertex> vertices;

    std::function<void()> onFlush;

public:
    PolygonShapeBuilder();

    void Reset();

    PolygonBatchVertex const* GetData() const noexcept;

    std::size_t GetVertexCount() const noexcept;

    bool IsEmpty() const noexcept;

    static constexpr std::size_t GetMaxVertexCount() noexcept {
        return MaxVertexCount;
    }

    void DrawArc(
        Vector2 const& position,
        float radius,
        Radian<float> const& startAngle,
        Radian<float> const& arcAngle,
        std::size_t segments,
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
};

} // namespace Pomdog

#endif // POMDOG_POLYGONSHAPEBUILDER_E6B8BC2B_HPP
