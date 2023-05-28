// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/math/color.h"
#include "pomdog/math/matrix3x2.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <functional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

struct PrimitiveBatchVertex final {
    // {xyz} = position.xyz
    Vector3 Position;

    // {xyzw} = color.rgba
    Vector4 Color;
};

class POMDOG_EXPORT PolygonShapeBuilder final {
private:
    using Vertex = PrimitiveBatchVertex;
    std::vector<PrimitiveBatchVertex> vertices;
    std::size_t maxVertexCount;
    std::size_t minVertexCount;
    std::function<void()> onFlush;

public:
    PolygonShapeBuilder();

    explicit PolygonShapeBuilder(std::size_t maxVertexCount);

    void Reset();

    const PrimitiveBatchVertex* GetData() const noexcept;

    std::size_t getVertexCount() const noexcept;

    bool IsEmpty() const noexcept;

    std::size_t GetMaxVertexCount() const noexcept;

    void DrawArc(
        const Vector2& position,
        float radius,
        const Radian<float>& startAngle,
        const Radian<float>& arcAngle,
        int segments,
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
        int segments,
        const Color& color);

    void DrawCircle(
        const Vector3& position,
        float radius,
        int segments,
        const Color& color);

    //void DrawEllipse();

    void DrawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& color,
        float weight);

    void DrawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& startColor,
        const Color& endColor,
        float weight);

    void DrawPolyline(
        const std::vector<Vector2>& points,
        float thickness,
        const Color& color);

    //void DrawPolygon(
    //    const std::vector<PrimitiveBatchVertex>& vertices,
    //    const Color& color);

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
        const Vector2& position,
        float width,
        float height,
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
    void DrawRectangle(
        const Matrix3x2& matrix,
        const Vector2& position,
        float width,
        float height,
        const Color& color1,
        const Color& color2,
        const Color& color3,
        const Color& color4);

    void DrawRectangle(
        const Vector2& position,
        float width,
        float height,
        const Vector2& originPivot,
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

    void DrawTriangle(
        const Vector3& point1,
        const Vector3& point2,
        const Vector3& point3,
        const Color& color1,
        const Color& color2,
        const Color& color3);

    void DrawTriangle(
        const Vector3& point1,
        const Vector3& point2,
        const Vector3& point3,
        const Vector4& color1,
        const Vector4& color2,
        const Vector4& color3);
};

} // namespace pomdog
