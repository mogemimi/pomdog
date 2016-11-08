// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Gameplay2D/PrimitiveRenderable.hpp"
#include "Pomdog.Experimental/Rendering/Renderer.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform.hpp"

namespace Pomdog {

PrimitiveRenderable::PrimitiveRenderable(
    const std::shared_ptr<GraphicsDevice>& graphicsDeviceIn)
    : graphicsDevice(graphicsDeviceIn)
{
    command.SetDrawOrder(0.0f);

    constexpr std::size_t MinVertexCount = 128;
    command.vertexBuffer = std::make_shared<VertexBuffer>(
        graphicsDevice,
        MinVertexCount,
        sizeof(PolygonBatchVertex),
        BufferUsage::Dynamic);

    command.constantBuffer = std::make_shared<ConstantBuffer>(
        graphicsDevice, sizeof(Matrix4x4), BufferUsage::Dynamic);
}

void PrimitiveRenderable::Visit(Entity & entity, Renderer & renderer)
{
    if (!IsVisible()) {
        return;
    }

    if (shapeBuilder.IsEmpty()) {
        return;
    }

    POMDOG_ASSERT(graphicsDevice);
    POMDOG_ASSERT(command.vertexBuffer);

    if (shapeBuilder.GetVertexCount() > command.vertexBuffer->GetVertexCount()) {
        // Reallocate vertex buffer
        command.vertexBuffer = std::make_shared<VertexBuffer>(
            graphicsDevice,
            shapeBuilder.GetVertexCount(),
            sizeof(PolygonBatchVertex),
            BufferUsage::Dynamic);
    }

    command.vertexBuffer->SetData(
        shapeBuilder.GetData(), shapeBuilder.GetVertexCount());
    command.vertexCount = shapeBuilder.GetVertexCount();
    command.SetDrawOrder(GetDrawOrder());

    if (auto transform = entity.GetComponent<Transform>()) {
        command.worldMatrix = transform->GetTransformMatrix();
    } else {
        command.worldMatrix = Matrix4x4::Identity;
    }
    renderer.PushCommand(command);
}

//void PrimitiveRenderable::DrawArc(
//    const Vector2& position,
//    float radius,
//    const Radian<float>& startAngle,
//    const Radian<float>& arcAngle,
//    int segments,
//    const Color& color)
//{
//}

//void PrimitiveRenderable::DrawBox(
//    const BoundingBox& box,
//    const Color& color)
//{
//}

//void PrimitiveRenderable::DrawBox(
//    const Vector3& position,
//    const Vector3& scale,
//    const Color& color)
//{
//}

//void PrimitiveRenderable::DrawBox(
//    const Vector3& position,
//    const Vector3& scale,
//    const Vector3& originPivot,
//    const Color& color)
//{
//}

void PrimitiveRenderable::DrawCircle(
    const Vector2& position,
    float radius,
    int segments,
    const Color& color)
{
    shapeBuilder.DrawCircle(position, radius, segments, color);
}

void PrimitiveRenderable::DrawCircle(
    const Vector3& position,
    float radius,
    int segments,
    const Color& color)
{
    shapeBuilder.DrawCircle(position, radius, segments, color);
}

void PrimitiveRenderable::DrawLine(
    const Vector2& start,
    const Vector2& end,
    const Color& color,
    float weight)
{
    shapeBuilder.DrawLine(start, end, color, weight);
}

void PrimitiveRenderable::DrawLine(
    const Vector2& start,
    const Vector2& end,
    const Color& startColor,
    const Color& endColor,
    float weight)
{
    shapeBuilder.DrawLine(start, end, startColor, endColor, weight);
}

//void PrimitiveRenderable::DrawPolyline(
//    const std::vector<Vector2>& points,
//    float thickness,
//    const Color& color)
//{
//}

void PrimitiveRenderable::DrawRectangle(
    const Rectangle& sourceRect,
    const Color& color)
{
    shapeBuilder.DrawRectangle(sourceRect, color);
}

void PrimitiveRenderable::DrawRectangle(
    const Rectangle& sourceRect,
    const Color& color1,
    const Color& color2,
    const Color& color3,
    const Color& color4)
{
    shapeBuilder.DrawRectangle(sourceRect, color1, color2, color3, color4);
}

void PrimitiveRenderable::DrawRectangle(
    const Matrix3x2& matrix,
    const Vector2& position,
    float width,
    float height,
    const Color& color)
{
    shapeBuilder.DrawRectangle(matrix, position, width, height, color);
}

void PrimitiveRenderable::DrawRectangle(
    const Matrix3x2& matrix,
    const Vector2& position,
    float width,
    float height,
    const Color& color1,
    const Color& color2,
    const Color& color3,
    const Color& color4)
{
    shapeBuilder.DrawRectangle(matrix, position, width, height, color1, color2, color3, color4);
}

void PrimitiveRenderable::DrawRectangle(
    const Vector2& position,
    float width,
    float height,
    const Vector2& originPivot,
    const Color& color)
{
    shapeBuilder.DrawRectangle(position, width, height, originPivot, color);
}

void PrimitiveRenderable::DrawSphere(
    const Vector3& position,
    float radius,
    const Color& color,
    int segments)
{
    shapeBuilder.DrawSphere(position, radius, color, segments);
}

//void PrimitiveRenderable::DrawTriangle(
//    const Vector2& point1,
//    const Vector2& point2,
//    const Vector2& point3,
//    const Color& color)
//{
//}

//void PrimitiveRenderable::DrawTriangle(
//    const Vector2& point1,
//    const Vector2& point2,
//    const Vector2& point3,
//    const Color& color1,
//    const Color& color2,
//    const Color& color3)
//{
//}

//void PrimitiveRenderable::DrawTriangle(
//    const Vector3& point1,
//    const Vector3& point2,
//    const Vector3& point3,
//    const Color& color1,
//    const Color& color2,
//    const Color& color3)
//{
//}

std::uint8_t ComponentTypeDeclaration<PrimitiveRenderable>::GetTypeIndex()
{
    return Detail::Gameplay::ComponentTypeIndex::Index<GraphicsComponent>();
}


ComponentCreator<PrimitiveRenderable>::ComponentCreator(
    const std::shared_ptr<GraphicsDevice>& graphicsDeviceIn)
    : graphicsDevice(graphicsDeviceIn)
{
}

std::shared_ptr<Component> ComponentCreator<PrimitiveRenderable>::CreateComponent()
{
    auto component = std::make_shared<PrimitiveRenderable>(graphicsDevice);
    return component;
}

std::uint8_t ComponentCreator<PrimitiveRenderable>::GetComponentType()
{
    return ComponentTypeDeclaration<PrimitiveRenderable>::GetTypeIndex();
}

} // namespace Pomdog
