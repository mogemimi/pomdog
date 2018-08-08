// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Graphics/PolygonBatch.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Experimental/Graphics/PolygonShapeBuilder.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/RasterizerDescription.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Math/BoundingBox.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include <cmath>
#include <cstring>

namespace Pomdog {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/LineBatch_PS.inc.hpp"
#include "Shaders/GLSL.Embedded/LineBatch_VS.inc.hpp"
#include "Shaders/HLSL.Embedded/LineBatch_PS.inc.hpp"
#include "Shaders/HLSL.Embedded/LineBatch_VS.inc.hpp"
#include "Shaders/Metal.Embedded/LineBatch.inc.hpp"

} // unnamed namespace

// MARK: - PolygonBatch::Impl

class PolygonBatch::Impl {
public:
    typedef PolygonBatchVertex Vertex;

private:
    std::shared_ptr<GraphicsCommandList> commandList;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<ConstantBuffer> constantBuffer;

public:
    PolygonShapeBuilder polygonShapes;
    std::size_t startVertexLocation;
    int drawCallCount;

public:
    Impl(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager & assets);

    void Begin(
        const std::shared_ptr<GraphicsCommandList>& commandListIn,
        const Matrix4x4& transformMatrix);

    void DrawTriangle(
        const Vector2& point1, const Vector2& point2, const Vector2& point3,
        const Vector4& color1, const Vector4& color2, const Vector4& color3);

    void End();

    void Flush();
};

PolygonBatch::Impl::Impl(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager & assets)
    : startVertexLocation(0)
    , drawCallCount(0)
{
    {
        auto maxVertexCount = polygonShapes.GetMaxVertexCount();
        vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
            maxVertexCount, sizeof(Vertex), BufferUsage::Dynamic);
    }
    {
        auto inputLayout = InputLayoutHelper{}
            .Float3().Float4();

        auto vertexShader = assets.CreateBuilder<Shader>(ShaderPipelineStage::VertexShader)
            .SetGLSL(Builtin_GLSL_LineBatch_VS, std::strlen(Builtin_GLSL_LineBatch_VS))
            .SetHLSLPrecompiled(BuiltinHLSL_LineBatch_VS, sizeof(BuiltinHLSL_LineBatch_VS))
            .SetMetal(Builtin_Metal_LineBatch, std::strlen(Builtin_Metal_LineBatch), "LineBatchVS");

        auto pixelShader = assets.CreateBuilder<Shader>(ShaderPipelineStage::PixelShader)
            .SetGLSL(Builtin_GLSL_LineBatch_PS, std::strlen(Builtin_GLSL_LineBatch_PS))
            .SetHLSLPrecompiled(BuiltinHLSL_LineBatch_PS, sizeof(BuiltinHLSL_LineBatch_PS))
            .SetMetal(Builtin_Metal_LineBatch, std::strlen(Builtin_Metal_LineBatch), "LineBatchPS");

        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        auto builder = assets.CreateBuilder<PipelineState>();
        pipelineState = builder
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetVertexShader(vertexShader.Build())
            .SetPixelShader(pixelShader.Build())
            .SetInputLayout(inputLayout.CreateInputLayout())
            .SetBlendState(BlendDescription::CreateNonPremultiplied())
            .SetDepthStencilState(DepthStencilDescription::CreateNone())
            .SetRasterizerState(RasterizerDescription::CreateCullCounterClockwise())
            .SetConstantBufferBindSlot("TransformMatrix", 0)
            .Build();
    }

    constantBuffer = std::make_shared<ConstantBuffer>(
        graphicsDevice, sizeof(Matrix4x4), BufferUsage::Dynamic);
}

void PolygonBatch::Impl::Begin(
    const std::shared_ptr<GraphicsCommandList>& commandListIn,
    const Matrix4x4& transformMatrix)
{
    POMDOG_ASSERT(commandListIn);
    commandList = commandListIn;

    alignas(16) Matrix4x4 transposedMatrix = Matrix4x4::Transpose(transformMatrix);
    constantBuffer->SetValue(transposedMatrix);

    startVertexLocation = 0;
    drawCallCount = 0;
}

void PolygonBatch::Impl::End()
{
    Flush();
    commandList.reset();
}

void PolygonBatch::Impl::Flush()
{
    if (polygonShapes.IsEmpty()) {
        return;
    }

    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(!polygonShapes.IsEmpty());
    POMDOG_ASSERT((startVertexLocation + polygonShapes.GetVertexCount()) <= polygonShapes.GetMaxVertexCount());

    const auto vertexOffsetBytes = sizeof(Vertex) * startVertexLocation;
    vertexBuffer->SetData(
        vertexOffsetBytes,
        polygonShapes.GetData(),
        polygonShapes.GetVertexCount(),
        sizeof(Vertex));

    commandList->SetVertexBuffer(vertexBuffer);
    commandList->SetPipelineState(pipelineState);
    commandList->SetConstantBuffer(0, constantBuffer);
    commandList->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
    commandList->Draw(polygonShapes.GetVertexCount(), startVertexLocation);

    startVertexLocation += polygonShapes.GetVertexCount();
    ++drawCallCount;

    polygonShapes.Reset();
}

// MARK: - PolygonBatch

PolygonBatch::PolygonBatch(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager & assets)
    : impl(std::make_unique<Impl>(graphicsDevice, assets))
{
}

PolygonBatch::~PolygonBatch() = default;

void PolygonBatch::Begin(
    const std::shared_ptr<GraphicsCommandList>& commandListIn,
    const Matrix4x4& transformMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->Begin(commandListIn, transformMatrixIn);
}

void PolygonBatch::End()
{
    POMDOG_ASSERT(impl);
    impl->End();
}

void PolygonBatch::DrawArc(
    const Vector2& position,
    float radius,
    const Radian<float>& startAngle,
    const Radian<float>& arcAngle,
    int segments,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawArc(
        position, radius, startAngle, arcAngle, segments, color);
}

void PolygonBatch::DrawBox(
    const BoundingBox& box,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawBox(box.Min, box.Max - box.Min, color);
}

void PolygonBatch::DrawBox(
    const Vector3& position,
    const Vector3& scale,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawBox(position, scale, color);
}

void PolygonBatch::DrawBox(
    const Vector3& position,
    const Vector3& scale,
    const Vector3& originPivot,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawBox(position, scale, originPivot, color);
}

void PolygonBatch::DrawCircle(
    const Vector2& position,
    float radius,
    int segments,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawCircle(position, radius, segments, color);
}

void PolygonBatch::DrawCircle(
    const Vector3& position,
    float radius,
    int segments,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawCircle(position, radius, segments, color);
}

void PolygonBatch::DrawLine(
    const Vector2& start,
    const Vector2& end,
    const Color& color,
    float weight)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawLine(start, end, color, weight);
}

void PolygonBatch::DrawLine(
    const Matrix3x2& matrix,
    const Vector2& start,
    const Vector2& end,
    const Color& color,
    float weight)
{
    POMDOG_ASSERT(impl);
    auto transformedStart = Vector2::Transform(start, matrix);
    auto transformedEnd = Vector2::Transform(end, matrix);
    impl->polygonShapes.DrawLine(transformedStart, transformedEnd, color, weight);
}

void PolygonBatch::DrawLine(
    const Vector2& start,
    const Vector2& end,
    const Color& startColor,
    const Color& endColor,
    float weight)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawLine(start, end, startColor, endColor, weight);
}

void PolygonBatch::DrawPolyline(
    const std::vector<Vector2>& points,
    float thickness,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawPolyline(points, thickness, color);
}

void PolygonBatch::DrawRectangle(
    const Rectangle& sourceRect,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawRectangle(sourceRect, color);
}

void PolygonBatch::DrawRectangle(
    const Rectangle& sourceRect,
    const Color& color1,
    const Color& color2,
    const Color& color3,
    const Color& color4)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawRectangle(
        sourceRect, color1, color2, color3, color4);
}

void PolygonBatch::DrawRectangle(
    const Matrix3x2& matrix,
    const Vector2& position,
    float width,
    float height,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawRectangle(matrix, position, width, height, color);
}

void PolygonBatch::DrawRectangle(
    const Matrix3x2& matrix,
    const Vector2& position,
    float width,
    float height,
    const Color& color1,
    const Color& color2,
    const Color& color3,
    const Color& color4)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawRectangle(
        matrix, position, width, height, color1, color2, color3, color4);
}

void PolygonBatch::DrawRectangle(
    const Vector2& position,
    float width,
    float height,
    const Vector2& originPivot,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawRectangle(
        position, width, height, originPivot, color);
}

void PolygonBatch::DrawSphere(
    const Vector3& position,
    float radius,
    const Color& color,
    int segments)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawSphere(
        position, radius, color, segments);
}

void PolygonBatch::DrawTriangle(
    const Vector2& point1,
    const Vector2& point2,
    const Vector2& point3,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawTriangle(point1, point2, point3, color);
}

void PolygonBatch::DrawTriangle(
    const Vector2& point1,
    const Vector2& point2,
    const Vector2& point3,
    const Color& color1,
    const Color& color2,
    const Color& color3)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawTriangle(
        point1, point2, point3, color1, color2, color3);
}

void PolygonBatch::DrawTriangle(
    const Vector3& point1,
    const Vector3& point2,
    const Vector3& point3,
    const Color& color1,
    const Color& color2,
    const Color& color3)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawTriangle(
        point1, point2, point3, color1, color2, color3);
}

void PolygonBatch::DrawTriangle(
    const Vector3& point1,
    const Vector3& point2,
    const Vector3& point3,
    const Vector4& color1,
    const Vector4& color2,
    const Vector4& color3)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawTriangle(
        point1, point2, point3, color1, color2, color3);
}

void PolygonBatch::Flush()
{
    POMDOG_ASSERT(impl);
    impl->Flush();
}

std::size_t PolygonBatch::GetMaxVertexCount() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->polygonShapes.GetMaxVertexCount();
}

int PolygonBatch::GetDrawCallCount() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->drawCallCount;
}

} // namespace Pomdog
