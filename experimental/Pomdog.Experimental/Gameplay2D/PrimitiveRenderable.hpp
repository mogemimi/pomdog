// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/detail/ComponentTypeIndex.hpp"
#include "Pomdog.Experimental/Gameplay2D/GraphicsComponent.hpp"
#include "Pomdog.Experimental/Rendering/Commands/PrimitiveCommand.hpp"
#include "Pomdog/Experimental/Graphics/PolygonShapeBuilder.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <Pomdog/Pomdog.hpp>
#include <functional>
#include <memory>

namespace Pomdog {

class PrimitiveRenderable final : public GraphicsComponent {
public:
    explicit PrimitiveRenderable(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    void Visit(Entity & entity, Renderer & renderer) override;

//    void DrawArc(
//        const Vector2& position,
//        float radius,
//        const Radian<float>& startAngle,
//        const Radian<float>& arcAngle,
//        int segments,
//        const Color& color);

//    void DrawBox(
//        const BoundingBox& box,
//        const Color& color);

//    void DrawBox(
//        const Vector3& position,
//        const Vector3& scale,
//        const Color& color);

//    void DrawBox(
//        const Vector3& position,
//        const Vector3& scale,
//        const Vector3& originPivot,
//        const Color& color);

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

//    void DrawPolyline(
//        const std::vector<Vector2>& points,
//        float thickness,
//        const Color& color);

//    //void DrawPolygon(
//    //    const std::vector<PolygonBatchVertex>& vertices,
//    //    const Color& color);

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

//    void DrawTriangle(
//        const Vector2& point1,
//        const Vector2& point2,
//        const Vector2& point3,
//        const Color& color);

//    void DrawTriangle(
//        const Vector2& point1,
//        const Vector2& point2,
//        const Vector2& point3,
//        const Color& color1,
//        const Color& color2,
//        const Color& color3);

//    void DrawTriangle(
//        const Vector3& point1,
//        const Vector3& point2,
//        const Vector3& point3,
//        const Color& color1,
//        const Color& color2,
//        const Color& color3);

private:
    PolygonShapeBuilder shapeBuilder;
    Pomdog::Rendering::PrimitiveCommand command;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
};

template <>
struct ComponentTypeDeclaration<PrimitiveRenderable> final {
    static std::uint8_t GetTypeIndex();
};

template <>
class ComponentCreator<PrimitiveRenderable> final : public ComponentCreatorBase {
private:
    std::shared_ptr<GraphicsDevice> graphicsDevice;

public:
    explicit ComponentCreator(const std::shared_ptr<GraphicsDevice>& graphicsDeviceIn);

    std::shared_ptr<Component> CreateComponent() override;

    std::uint8_t GetComponentType() override;
};

} // namespace Pomdog
