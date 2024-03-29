// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/polygon_shape_builder.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

constexpr std::size_t DefaultMaxVertexCount = 4096 * 8;
constexpr std::size_t DefaultMinVertexCount = 256;

} // namespace

PolygonShapeBuilder::PolygonShapeBuilder()
    : maxVertexCount(DefaultMaxVertexCount)
    , minVertexCount(DefaultMinVertexCount)
{
    POMDOG_ASSERT(minVertexCount <= maxVertexCount);
    vertices.reserve(minVertexCount);
}

PolygonShapeBuilder::PolygonShapeBuilder(std::size_t maxVertexCountIn)
    : maxVertexCount(maxVertexCountIn)
    , minVertexCount(1)
{
    POMDOG_ASSERT(3 <= maxVertexCount);
    POMDOG_ASSERT(minVertexCount <= maxVertexCount);
}

void PolygonShapeBuilder::reset()
{
    POMDOG_ASSERT(minVertexCount <= maxVertexCount);
    if (vertices.capacity() > maxVertexCount) {
        POMDOG_ASSERT(vertices.size() <= maxVertexCount);
        vertices.resize(maxVertexCount);
        vertices.shrink_to_fit();
    }
    vertices.clear();
}

const PrimitiveBatchVertex* PolygonShapeBuilder::getData() const noexcept
{
    return vertices.data();
}

std::size_t PolygonShapeBuilder::getVertexCount() const noexcept
{
    return vertices.size();
}

bool PolygonShapeBuilder::isEmpty() const noexcept
{
    return vertices.empty();
}

std::size_t PolygonShapeBuilder::getMaxVertexCount() const noexcept
{
    return maxVertexCount;
}

void PolygonShapeBuilder::drawArc(
    const Vector2& position,
    float radius,
    const Radian<float>& startAngle,
    const Radian<float>& arcAngle,
    int segments,
    const Color& color)
{
    POMDOG_ASSERT(radius >= 0);
    POMDOG_ASSERT(arcAngle >= 0);
    POMDOG_ASSERT(segments >= 3);

    if (radius <= 0 || segments < 3 || arcAngle <= 0) {
        return;
    }

    POMDOG_ASSERT(radius > 0);
    POMDOG_ASSERT(arcAngle >= 0);
    POMDOG_ASSERT(segments >= 3);

    const auto center = Vector3{position, 0.0f};
    const Radian<float> centralAngle =
        std::min(arcAngle.value, math::TwoPi<float>) / segments;

    auto computePoint = [&](int index) {
        auto rad = startAngle + centralAngle * static_cast<float>(index);
        auto cos = std::cos(rad.value);
        auto sin = std::sin(rad.value);
        return center + Vector3{radius * Vector2{cos, sin}, 0};
    };

    auto prevPoint = computePoint(0);
    auto colorVector = color.toVector4();

    for (int i = 0; i < segments; ++i) {
        auto nextPoint = computePoint(i + 1);
        drawTriangle(nextPoint, prevPoint, center,
            colorVector, colorVector, colorVector);
        prevPoint = nextPoint;
    }
}

void PolygonShapeBuilder::drawBox(
    const Vector3& position,
    const Vector3& scale,
    const Color& color)
{
    drawBox(position, scale, Vector3{0.0f, 0.0f, 0.0f}, color);
}

void PolygonShapeBuilder::drawBox(
    const Vector3& position,
    const Vector3& scale,
    const Vector3& originPivot,
    const Color& color)
{
    Vector3 boxVertices[] = {
        // top
        Vector3(0.0f, 1.0f, 0.0f),
        Vector3(1.0f, 1.0f, 0.0f),
        Vector3(1.0f, 1.0f, 1.0f),
        Vector3(0.0f, 1.0f, 1.0f),

        // bottom
        Vector3(0.0f, 0.0f, 0.0f),
        Vector3(1.0f, 0.0f, 0.0f),
        Vector3(1.0f, 0.0f, 1.0f),
        Vector3(0.0f, 0.0f, 1.0f),

        // left
        Vector3(0.0f, 0.0f, 1.0f),
        Vector3(0.0f, 0.0f, 0.0f),
        Vector3(0.0f, 1.0f, 0.0f),
        Vector3(0.0f, 1.0f, 1.0f),

        // right
        Vector3(1.0f, 0.0f, 1.0f),
        Vector3(1.0f, 0.0f, 0.0f),
        Vector3(1.0f, 1.0f, 0.0f),
        Vector3(1.0f, 1.0f, 1.0f),

        // front
        Vector3(0.0f, 0.0f, 0.0f),
        Vector3(1.0f, 0.0f, 0.0f),
        Vector3(1.0f, 1.0f, 0.0f),
        Vector3(0.0f, 1.0f, 0.0f),

        // back
        Vector3(0.0f, 0.0f, 1.0f),
        Vector3(1.0f, 0.0f, 1.0f),
        Vector3(1.0f, 1.0f, 1.0f),
        Vector3(0.0f, 1.0f, 1.0f),
    };

    for (auto& v : boxVertices) {
        v = ((v - originPivot) * scale) + position;
    }

    const auto colorVector = color.toVector4();
    auto draw = [&](int a, int b, int c) {
        drawTriangle(
            boxVertices[a],
            boxVertices[b],
            boxVertices[c],
            colorVector,
            colorVector,
            colorVector);
    };

    draw(3, 1, 0);
    draw(2, 1, 3);
    draw(6, 4, 5);
    draw(7, 4, 6);
    draw(11, 9, 8);
    draw(10, 9, 11);
    draw(14, 12, 13);
    draw(15, 12, 14);
    draw(19, 17, 16);
    draw(18, 17, 19);
    draw(22, 20, 21);
    draw(23, 20, 22);
}

void PolygonShapeBuilder::drawCircle(
    const Vector2& position,
    float radius,
    int segments,
    const Color& color)
{
    POMDOG_ASSERT(segments >= 3);
    POMDOG_ASSERT(radius >= 0);

    drawCircle(Vector3{position, 0.0f}, radius, segments, color);
}

void PolygonShapeBuilder::drawCircle(
    const Vector3& position,
    float radius,
    int segments,
    const Color& color)
{
    POMDOG_ASSERT(segments >= 3);
    POMDOG_ASSERT(radius >= 0);

    if (radius <= 0) {
        return;
    }

    if (segments < 3) {
        return;
    }

    POMDOG_ASSERT(radius > 0);
    POMDOG_ASSERT(segments >= 3);

    Radian<float> centralAngle = math::TwoPi<float> / segments;
    auto prevPoint = position + Vector3{radius, 0, 0};

    auto colorVector = color.toVector4();

    for (int i = 0; i < segments; ++i) {
        auto rad = centralAngle * static_cast<float>(i + 1);
        auto cos = std::cos(rad.value);
        auto sin = std::sin(rad.value);
        auto nextPoint = position + Vector3{radius * Vector2{cos, sin}, 0};
        drawTriangle(nextPoint, prevPoint, position,
            colorVector, colorVector, colorVector);
        prevPoint = nextPoint;
    }
}

void PolygonShapeBuilder::drawLine(
    const Vector2& start,
    const Vector2& end,
    const Color& color,
    float weight)
{
    drawLine(start, end, color, color, weight);
}

void PolygonShapeBuilder::drawLine(
    const Vector2& start,
    const Vector2& end,
    const Color& startColor,
    const Color& endColor,
    float weight)
{
    auto halfWeight = weight / 2;

    if (halfWeight <= 0) {
        return;
    }

    auto vec = end - start;
    std::swap(vec.x, vec.y);

    auto a = math::normalize(vec * Vector2{1, -1}) * halfWeight;
    auto b = math::normalize(vec * Vector2{-1, 1}) * halfWeight;

    Vector3 p1 = Vector3{start + a, 0.0f};
    Vector3 p2 = Vector3{start + b, 0.0f};
    Vector3 p3 = Vector3{end + b, 0.0f};
    Vector3 p4 = Vector3{end + a, 0.0f};

    drawTriangle(p1, p2, p3, startColor, startColor, endColor);
    drawTriangle(p3, p4, p1, endColor, endColor, startColor);
}

void PolygonShapeBuilder::drawPolyline(
    const std::vector<Vector2>& points,
    float thickness,
    const Color& color)
{
    if (points.size() < 2) {
        return;
    }

    auto halfThickness = thickness / 2;

    if (halfThickness <= 0) {
        return;
    }

    ///@todo Please reimplementation this function:

    for (std::size_t i = 0; i + 1 < points.size(); ++i) {
        auto& start = points[i];
        auto& end = points[i + 1];

        auto vec = end - start;
        std::swap(vec.x, vec.y);

        auto a = math::normalize(vec * Vector2{1, -1}) * halfThickness;
        auto b = math::normalize(vec * Vector2{-1, 1}) * halfThickness;

        Vector3 p1 = Vector3{start + a, 0.0f};
        Vector3 p2 = Vector3{start + b, 0.0f};
        Vector3 p3 = Vector3{end + b, 0.0f};
        Vector3 p4 = Vector3{end + a, 0.0f};

        drawTriangle(p1, p2, p3, color, color, color);
        drawTriangle(p3, p4, p1, color, color, color);
    }
}

void PolygonShapeBuilder::drawRectangle(
    const Rectangle& sourceRect,
    const Color& color)
{
    drawRectangle(sourceRect, color, color, color, color);
}

void PolygonShapeBuilder::drawRectangle(
    const Rectangle& sourceRect,
    const Color& color1,
    const Color& color2,
    const Color& color3,
    const Color& color4)
{
    if (sourceRect.width <= 0 || sourceRect.height <= 0) {
        return;
    }

    std::array<Vector3, 4> rectVertices = {{
        Vector3{static_cast<float>(sourceRect.getLeft()), static_cast<float>(sourceRect.y), 0.0f},
        Vector3{static_cast<float>(sourceRect.getLeft()), static_cast<float>(sourceRect.y + sourceRect.height), 0.0f},
        Vector3{static_cast<float>(sourceRect.getRight()), static_cast<float>(sourceRect.y + sourceRect.height), 0.0f},
        Vector3{static_cast<float>(sourceRect.getRight()), static_cast<float>(sourceRect.y), 0.0f},
    }};

    const auto colorVector1 = color1.toVector4();
    const auto colorVector2 = color2.toVector4();
    const auto colorVector3 = color3.toVector4();
    const auto colorVector4 = color4.toVector4();

    drawTriangle(
        rectVertices[0], rectVertices[1], rectVertices[2],
        colorVector1, colorVector4, colorVector3);
    drawTriangle(
        rectVertices[2], rectVertices[3], rectVertices[0],
        colorVector3, colorVector2, colorVector1);
}

void PolygonShapeBuilder::drawRectangle(
    const Matrix3x2& matrix,
    const Vector2& position,
    float width,
    float height,
    const Color& color)
{
    drawRectangle(matrix, position, width, height, color, color, color, color);
}

void PolygonShapeBuilder::drawRectangle(
    const Matrix3x2& matrix,
    const Vector2& position,
    float width,
    float height,
    const Color& color1,
    const Color& color2,
    const Color& color3,
    const Color& color4)
{
    if (width <= 0 || height <= 0) {
        return;
    }

    const auto left = position.x;
    const auto right = position.x + width;

    std::array<Vector3, 4> rectVertices = {{
        Vector3{left, position.y, 0.0f},
        Vector3{left, position.y + height, 0.0f},
        Vector3{right, position.y + height, 0.0f},
        Vector3{right, position.y, 0.0f},
    }};

    for (auto& vertex : rectVertices) {
        const auto vec2 = math::transform(Vector2{vertex.x, vertex.y}, matrix);
        vertex.x = vec2.x;
        vertex.y = vec2.y;
    }

    const auto colorVector1 = color1.toVector4();
    const auto colorVector2 = color2.toVector4();
    const auto colorVector3 = color3.toVector4();
    const auto colorVector4 = color4.toVector4();

    drawTriangle(
        rectVertices[0], rectVertices[1], rectVertices[2],
        colorVector1, colorVector4, colorVector3);
    drawTriangle(
        rectVertices[2], rectVertices[3], rectVertices[0],
        colorVector3, colorVector2, colorVector1);
}

void PolygonShapeBuilder::drawRectangle(
    const Vector2& position,
    float width,
    float height,
    const Vector2& originPivot,
    const Color& color)
{
    if (width <= 0 || height <= 0) {
        return;
    }

    Vector2 anchorOffset = Vector2{width, height} * originPivot;
    const auto minX = position.x - anchorOffset.x;
    const auto minY = position.y - anchorOffset.y;
    const auto maxX = position.x + width - anchorOffset.x;
    const auto maxY = position.y + height - anchorOffset.y;

    std::array<Vector3, 4> rectVertices = {{
        Vector3{minX, minY, 0.0f},
        Vector3{minX, maxY, 0.0f},
        Vector3{maxX, maxY, 0.0f},
        Vector3{maxX, minY, 0.0f},
    }};

    const auto colorVector = color.toVector4();

    drawTriangle(
        rectVertices[0], rectVertices[1], rectVertices[2],
        colorVector, colorVector, colorVector);
    drawTriangle(
        rectVertices[2], rectVertices[3], rectVertices[0],
        colorVector, colorVector, colorVector);
}

void PolygonShapeBuilder::drawSphere(
    const Vector3& position,
    float radius,
    const Color& color,
    int segments)
{
    POMDOG_ASSERT(segments >= 4);
    POMDOG_ASSERT(radius >= 0);

    if (radius <= 0) {
        return;
    }

    const auto rings = std::max(segments, 4);
    const auto sectors = std::max(segments, 4);

    std::vector<Vector3> sphereVertices;
    sphereVertices.reserve(rings * sectors);

    const auto R = 1.0f / static_cast<float>(rings - 1);
    const auto S = 1.0f / static_cast<float>(sectors - 1);

    for (int ring = 0; ring < rings; ++ring) {
        const auto latitude = math::Pi<float> * ring * R;
        const auto y = std::cos(latitude);
        const auto r = std::sin(latitude);
        for (int s = 0; s < sectors; ++s) {
            const auto longitude = math::TwoPi<float> * s * S;
            const auto x = r * std::cos(longitude);
            const auto z = r * std::sin(longitude);
            sphereVertices.push_back(Vector3{x, y, z} * radius + position);
        }
    }

    const auto colorVector = color.toVector4();
    const auto drawIndices = [&](std::size_t a, std::size_t b, std::size_t c, std::size_t d) {
        POMDOG_ASSERT(a < sphereVertices.size());
        POMDOG_ASSERT(b < sphereVertices.size());
        POMDOG_ASSERT(c < sphereVertices.size());
        POMDOG_ASSERT(d < sphereVertices.size());
        drawTriangle(
            sphereVertices[a],
            sphereVertices[c],
            sphereVertices[b],
            colorVector,
            colorVector,
            colorVector);
        drawTriangle(
            sphereVertices[c],
            sphereVertices[a],
            sphereVertices[d],
            colorVector,
            colorVector,
            colorVector);
    };

    for (int r = 0; r < rings - 1; ++r) {
        for (int s = 0; s < sectors - 1; ++s) {
            drawIndices(
                r * sectors + s,
                (r + 1) * sectors + s,
                (r + 1) * sectors + (s + 1),
                r * sectors + (s + 1));
        }
    }
}

void PolygonShapeBuilder::drawTriangle(
    const Vector2& point1,
    const Vector2& point2,
    const Vector2& point3,
    const Color& color)
{
    auto colorVector = color.toVector4();
    drawTriangle(
        Vector3{point1, 0.0f},
        Vector3{point2, 0.0f},
        Vector3{point3, 0.0f},
        colorVector, colorVector, colorVector);
}

void PolygonShapeBuilder::drawTriangle(
    const Vector2& point1,
    const Vector2& point2,
    const Vector2& point3,
    const Color& color1,
    const Color& color2,
    const Color& color3)
{
    drawTriangle(
        Vector3{point1, 0.0f},
        Vector3{point2, 0.0f},
        Vector3{point3, 0.0f},
        color1.toVector4(),
        color2.toVector4(),
        color3.toVector4());
}

void PolygonShapeBuilder::drawTriangle(
    const Vector3& point1,
    const Vector3& point2,
    const Vector3& point3,
    const Color& color1,
    const Color& color2,
    const Color& color3)
{
    drawTriangle(point1, point2, point3,
        color1.toVector4(), color2.toVector4(), color3.toVector4());
}

void PolygonShapeBuilder::drawTriangle(
    const Vector3& point1,
    const Vector3& point2,
    const Vector3& point3,
    const Vector4& color1,
    const Vector4& color2,
    const Vector4& color3)
{
    POMDOG_ASSERT(minVertexCount <= maxVertexCount);
    POMDOG_ASSERT(3 < maxVertexCount);

    if (vertices.size() + 3 > maxVertexCount) {
        if (onFlush) {
            onFlush();
        }
    }

    POMDOG_ASSERT(vertices.size() + 3 <= maxVertexCount);
    vertices.push_back(Vertex{point1, color1});
    vertices.push_back(Vertex{point2, color2});
    vertices.push_back(Vertex{point3, color3});
}

} // namespace pomdog
