// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "PolygonBatch.hpp"
#include "Pomdog.Experimental/Graphics/PolygonShapeBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include <cmath>

namespace Pomdog {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/LineBatch_VS.inc.hpp"
#include "Shaders/GLSL.Embedded/LineBatch_PS.inc.hpp"
#include "Shaders/HLSL.Embedded/LineBatch_VS.inc.hpp"
#include "Shaders/HLSL.Embedded/LineBatch_PS.inc.hpp"

} // unnamed namespace
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - PolygonBatch::Impl
#endif
//-----------------------------------------------------------------------
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
    std::uint32_t drawCallCount;

public:
    Impl(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets);

    void Begin(
        std::shared_ptr<GraphicsCommandList> const& commandListIn,
        Matrix4x4 const& transformMatrix);

    void DrawTriangle(
        Vector2 const& point1, Vector2 const& point2, Vector2 const& point3,
        Vector4 const& color1, Vector4 const& color2, Vector4 const& color3);

    void End();

    void Flush();
};
//-----------------------------------------------------------------------
PolygonBatch::Impl::Impl(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
    : drawCallCount(0)
{
    {
        auto maxVertexCount = polygonShapes.GetMaxVertexCount();
        vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
            maxVertexCount, sizeof(Vertex), BufferUsage::Dynamic);
    }
    {
        auto inputLayout = InputLayoutHelper{}
            .Float3().Float4();

        auto vertexShader = assets.CreateBuilder<Shader>()
            .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::VertexShader)
            .SetGLSL(Builtin_GLSL_LineBatch_VS, std::strlen(Builtin_GLSL_LineBatch_VS))
            .SetHLSLPrecompiled(BuiltinHLSL_LineBatch_VS, sizeof(BuiltinHLSL_LineBatch_VS));

        auto pixelShader = assets.CreateBuilder<Shader>()
            .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::PixelShader)
            .SetGLSL(Builtin_GLSL_LineBatch_PS, std::strlen(Builtin_GLSL_LineBatch_PS))
            .SetHLSLPrecompiled(BuiltinHLSL_LineBatch_PS, sizeof(BuiltinHLSL_LineBatch_PS));

        auto builder = assets.CreateBuilder<PipelineState>();
        pipelineState = builder
            .SetVertexShader(vertexShader.Build())
            .SetPixelShader(pixelShader.Build())
            .SetInputLayout(inputLayout.CreateInputLayout())
            .SetBlendState(BlendDescription::CreateNonPremultiplied())
            .SetDepthStencilState(DepthStencilDescription::CreateNone())
            .SetConstantBufferBindSlot("TransformMatrix", 0)
            .Build();
    }

    constantBuffer = std::make_shared<ConstantBuffer>(
        graphicsDevice, sizeof(Matrix4x4), BufferUsage::Dynamic);
}
//-----------------------------------------------------------------------
void PolygonBatch::Impl::Begin(
    std::shared_ptr<GraphicsCommandList> const& commandListIn,
    Matrix4x4 const& transformMatrix)
{
    POMDOG_ASSERT(commandListIn);
    commandList = commandListIn;

    alignas(16) Matrix4x4 transposed = Matrix4x4::Transpose(transformMatrix);
    constantBuffer->SetValue(transposed);

    drawCallCount = 0;
}
//-----------------------------------------------------------------------
void PolygonBatch::Impl::End()
{
    if (polygonShapes.IsEmpty()) {
        return;
    }

    Flush();
}
//-----------------------------------------------------------------------
void PolygonBatch::Impl::Flush()
{
    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(!polygonShapes.IsEmpty());
    POMDOG_ASSERT(polygonShapes.GetVertexCount() <= polygonShapes.GetMaxVertexCount());
    vertexBuffer->SetData(polygonShapes.GetData(), polygonShapes.GetVertexCount());

    commandList->SetVertexBuffer(vertexBuffer);
    commandList->SetPipelineState(pipelineState);
    commandList->SetConstantBuffer(0, constantBuffer);
    commandList->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
    commandList->Draw(polygonShapes.GetVertexCount());

    ++drawCallCount;

    polygonShapes.Reset();
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - PolygonBatch
#endif
//-----------------------------------------------------------------------
PolygonBatch::PolygonBatch(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
    : impl(std::make_unique<Impl>(graphicsDevice, assets))
{}
//-----------------------------------------------------------------------
PolygonBatch::~PolygonBatch() = default;
//-----------------------------------------------------------------------
void PolygonBatch::Begin(
    std::shared_ptr<GraphicsCommandList> const& commandListIn,
    Matrix4x4 const& transformMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->Begin(commandListIn, transformMatrixIn);
}
//-----------------------------------------------------------------------
void PolygonBatch::End()
{
    POMDOG_ASSERT(impl);
    impl->End();
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawArc(
    Vector2 const& position,
    float radius,
    Radian<float> const& startAngle,
    Radian<float> const& arcAngle,
    std::size_t segments,
    Color const& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawArc(
        position, radius, startAngle, arcAngle, segments, color);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawCircle(
    Vector2 const& position,
    float radius,
    std::size_t segments,
    Color const& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawCircle(position, radius, segments, color);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawCircle(
    Vector3 const& position,
    float radius,
    std::size_t segments,
    Color const& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawCircle(position, radius, segments, color);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawLine(
    Vector2 const& start,
    Vector2 const& end,
    Color const& color,
    float weight)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawLine(start, end, color, weight);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawLine(
    Vector2 const& start,
    Vector2 const& end,
    Color const& startColor,
    Color const& endColor,
    float weight)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawLine(start, end, startColor, endColor, weight);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawPolyline(
    std::vector<Vector2> const& points,
    float thickness,
    Color const& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawPolyline(points, thickness, color);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawRectangle(
    Rectangle const& sourceRect,
    Color const& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawRectangle(sourceRect, color);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawRectangle(
    Rectangle const& sourceRect,
    Color const& color1,
    Color const& color2,
    Color const& color3,
    Color const& color4)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawRectangle(
        sourceRect, color1, color2, color3, color4);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawRectangle(
    Matrix3x2 const& matrix,
    Rectangle const& sourceRect,
    Color const& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawRectangle(matrix, sourceRect, color);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawRectangle(
    Matrix3x2 const& matrix,
    Rectangle const& sourceRect,
    Color const& color1,
    Color const& color2,
    Color const& color3,
    Color const& color4)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawRectangle(
        matrix, sourceRect, color1, color2, color3, color4);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawRectangle(
    Vector2 const& position,
    float width,
    float height,
    Vector2 const& originPivot,
    Color const& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawRectangle(
        position, width, height, originPivot, color);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawTriangle(
    Vector2 const& point1,
    Vector2 const& point2,
    Vector2 const& point3,
    Color const& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawTriangle(point1, point2, point3, color);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawTriangle(
    Vector2 const& point1,
    Vector2 const& point2,
    Vector2 const& point3,
    Color const& color1,
    Color const& color2,
    Color const& color3)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawTriangle(
        point1, point2, point3, color1, color2, color3);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawTriangle(
    Vector3 const& point1,
    Vector3 const& point2,
    Vector3 const& point3,
    Color const& color1,
    Color const& color2,
    Color const& color3)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawTriangle(
        point1, point2, point3, color1, color2, color3);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawTriangle(
    Vector3 const& point1,
    Vector3 const& point2,
    Vector3 const& point3,
    Vector4 const& color1,
    Vector4 const& color2,
    Vector4 const& color3)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawTriangle(
        point1, point2, point3, color1, color2, color3);
}
//-----------------------------------------------------------------------
std::size_t PolygonBatch::GetMaxVertexCount() const noexcept
{
    return PolygonShapeBuilder::GetMaxVertexCount();
}
//-----------------------------------------------------------------------
std::uint32_t PolygonBatch::GetDrawCallCount() const
{
    POMDOG_ASSERT(impl);
    return impl->drawCallCount;
}
//-----------------------------------------------------------------------
} // namespace Pomdog
