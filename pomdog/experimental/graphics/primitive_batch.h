// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/math/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class AssetManager;

class POMDOG_EXPORT PrimitiveBatch final {
public:
    PrimitiveBatch(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        AssetManager& assets);

    PrimitiveBatch(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<gpu::DepthStencilDescriptor>&& depthStencilDesc,
        std::optional<gpu::RasterizerDescriptor>&& rasterizerDesc,
        AssetManager& assets);

    ~PrimitiveBatch();

    void begin(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const Matrix4x4& transformMatrix);

    void drawArc(
        const Vector2& position,
        float radius,
        const Radian<float>& startAngle,
        const Radian<float>& arcAngle,
        int segments,
        const Color& color);

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
        float radius,
        int segments,
        const Color& color);

    void drawCircle(
        const Vector3& position,
        float radius,
        int segments,
        const Color& color);

    // void drawEllipse();

    void drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& color,
        float weight);

    void drawLine(
        const Matrix3x2& matrix,
        const Vector2& start,
        const Vector2& end,
        const Color& color,
        float weight);

    void drawLine(
        const Vector2& start,
        const Vector2& end,
        const Color& startColor,
        const Color& endColor,
        float weight);

    void drawPolyline(
        const std::vector<Vector2>& points,
        float thickness,
        const Color& color);

    // void drawPolygon(
    //    const std::vector<PrimitiveBatchVertex>& vertices,
    //    const Color& color);

    void drawRectangle(
        const Rectangle& sourceRect,
        const Color& color);

    void drawRectangle(
        const Rectangle& sourceRect,
        const Color& color1,
        const Color& color2,
        const Color& color3,
        const Color& color4);

    void drawRectangle(
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
    void drawRectangle(
        const Matrix3x2& matrix,
        const Vector2& position,
        float width,
        float height,
        const Color& color1,
        const Color& color2,
        const Color& color3,
        const Color& color4);

    void drawRectangle(
        const Vector2& position,
        float width,
        float height,
        const Vector2& originPivot,
        const Color& color);

    void drawSphere(
        const Vector3& position,
        float radius,
        const Color& color,
        int segments);

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

    void drawTriangle(
        const Vector3& point1,
        const Vector3& point2,
        const Vector3& point3,
        const Color& color1,
        const Color& color2,
        const Color& color3);

    void drawTriangle(
        const Vector3& point1,
        const Vector3& point2,
        const Vector3& point3,
        const Vector4& color1,
        const Vector4& color2,
        const Vector4& color3);

    void flush();

    void end();

    [[nodiscard]] std::size_t getMaxVertexCount() const noexcept;

    [[nodiscard]] int getDrawCallCount() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog
