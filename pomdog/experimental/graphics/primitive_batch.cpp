// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/asset_builders/shader_builder.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/experimental/graphics/polygon_shape_builder.h"
#include "pomdog/gpu/blend_descriptor.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/depth_stencil_descriptor.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/input_layout_helper.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/rasterizer_descriptor.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/math/bounding_box.h"
#include "pomdog/math/color.h"
#include "pomdog/math/math.h"
#include "pomdog/math/matrix3x2.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

// Built-in shaders
#include "shaders/glsl.embedded/primitive_batch_ps.inc.h"
#include "shaders/glsl.embedded/primitive_batch_vs.inc.h"
#include "shaders/hlsl.embedded/primitive_batch_ps.inc.h"
#include "shaders/hlsl.embedded/primitive_batch_vs.inc.h"
#include "shaders/metal.embedded/primitive_batch.inc.h"

} // namespace

// MARK: - PrimitiveBatch::Impl

class PrimitiveBatch::Impl {
public:
    using Vertex = PrimitiveBatchVertex;

private:
    std::shared_ptr<gpu::CommandList> commandList;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer;
    std::shared_ptr<gpu::PipelineState> pipelineState;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer;

public:
    PolygonShapeBuilder polygonShapes;
    std::size_t startVertexLocation;
    int drawCallCount;

public:
    Impl(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<gpu::DepthStencilDescriptor>&& depthStencilDesc,
        std::optional<gpu::RasterizerDescriptor>&& rasterizerDesc,
        AssetManager& assets);

    void Begin(
        const std::shared_ptr<gpu::CommandList>& commandListIn,
        const Matrix4x4& transformMatrix);

    void DrawTriangle(
        const Vector2& point1, const Vector2& point2, const Vector2& point3,
        const Vector4& color1, const Vector4& color2, const Vector4& color3);

    void End();

    void Flush();
};

PrimitiveBatch::Impl::Impl(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::DepthStencilDescriptor>&& depthStencilDesc,
    std::optional<gpu::RasterizerDescriptor>&& rasterizerDesc,
    AssetManager& assets)
    : startVertexLocation(0)
    , drawCallCount(0)
{
    if (!depthStencilDesc) {
        depthStencilDesc = gpu::DepthStencilDescriptor::CreateNone();
    }
    if (!rasterizerDesc) {
        rasterizerDesc = gpu::RasterizerDescriptor::CreateCullCounterClockwise();
    }

    POMDOG_ASSERT(depthStencilDesc);
    POMDOG_ASSERT(rasterizerDesc);

    {
        auto maxVertexCount = polygonShapes.GetMaxVertexCount();
        vertexBuffer = std::get<0>(graphicsDevice->CreateVertexBuffer(
            maxVertexCount,
            sizeof(Vertex),
            gpu::BufferUsage::Dynamic));
    }
    {
        auto inputLayout = gpu::InputLayoutHelper{}
            .Float3().Float4();

        auto [vertexShader, vertexShaderErr] = assets.CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader)
            .SetGLSL(Builtin_GLSL_PrimitiveBatch_VS, std::strlen(Builtin_GLSL_PrimitiveBatch_VS))
            .SetHLSLPrecompiled(BuiltinHLSL_PrimitiveBatch_VS, sizeof(BuiltinHLSL_PrimitiveBatch_VS))
            .SetMetal(Builtin_Metal_PrimitiveBatch, std::strlen(Builtin_Metal_PrimitiveBatch), "PrimitiveBatchVS")
            .Build();

        if (vertexShaderErr != nullptr) {
            // FIXME: error handling
        }

        auto [pixelShader, pixelShaderErr] = assets.CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader)
            .SetGLSL(Builtin_GLSL_PrimitiveBatch_PS, std::strlen(Builtin_GLSL_PrimitiveBatch_PS))
            .SetHLSLPrecompiled(BuiltinHLSL_PrimitiveBatch_PS, sizeof(BuiltinHLSL_PrimitiveBatch_PS))
            .SetMetal(Builtin_Metal_PrimitiveBatch, std::strlen(Builtin_Metal_PrimitiveBatch), "PrimitiveBatchPS")
            .Build();

        if (pixelShaderErr != nullptr) {
            // FIXME: error handling
        }

        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        std::unique_ptr<Error> pipelineStateErr;
        std::tie(pipelineState, pipelineStateErr) = assets.CreateBuilder<gpu::PipelineState>()
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetVertexShader(std::move(vertexShader))
            .SetPixelShader(std::move(pixelShader))
            .SetInputLayout(inputLayout.CreateInputLayout())
            .SetPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
            .SetBlendState(gpu::BlendDescriptor::CreateNonPremultiplied())
            .SetDepthStencilState(*depthStencilDesc)
            .SetRasterizerState(*rasterizerDesc)
            .SetConstantBufferBindSlot("TransformMatrix", 0)
            .Build();

        if (pipelineStateErr != nullptr) {
            // FIXME: error handling
        }
    }

    constantBuffer = std::get<0>(graphicsDevice->CreateConstantBuffer(
        sizeof(Matrix4x4),
        gpu::BufferUsage::Dynamic));
}

void PrimitiveBatch::Impl::Begin(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const Matrix4x4& transformMatrix)
{
    POMDOG_ASSERT(commandListIn);
    commandList = commandListIn;

    alignas(16) Matrix4x4 transposedMatrix = math::Transpose(transformMatrix);
    constantBuffer->SetValue(transposedMatrix);

    startVertexLocation = 0;
    drawCallCount = 0;
}

void PrimitiveBatch::Impl::End()
{
    Flush();
    commandList.reset();
}

void PrimitiveBatch::Impl::Flush()
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

    commandList->SetVertexBuffer(0, vertexBuffer);
    commandList->SetPipelineState(pipelineState);
    commandList->SetConstantBuffer(0, constantBuffer);
    commandList->Draw(polygonShapes.GetVertexCount(), startVertexLocation);

    startVertexLocation += polygonShapes.GetVertexCount();
    ++drawCallCount;

    polygonShapes.Reset();
}

// MARK: - PrimitiveBatch

PrimitiveBatch::PrimitiveBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
    : PrimitiveBatch(graphicsDevice, std::nullopt, std::nullopt, assets)
{
}

PrimitiveBatch::PrimitiveBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::DepthStencilDescriptor>&& depthStencilDesc,
    std::optional<gpu::RasterizerDescriptor>&& rasterizerDesc,
    AssetManager& assets)
    : impl(std::make_unique<Impl>(
          graphicsDevice,
          std::move(depthStencilDesc),
          std::move(rasterizerDesc),
          assets))
{
}

PrimitiveBatch::~PrimitiveBatch() = default;

void PrimitiveBatch::Begin(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const Matrix4x4& transformMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->Begin(commandListIn, transformMatrixIn);
}

void PrimitiveBatch::End()
{
    POMDOG_ASSERT(impl);
    impl->End();
}

void PrimitiveBatch::DrawArc(
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

void PrimitiveBatch::DrawBox(
    const BoundingBox& box,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawBox(box.Min, box.Max - box.Min, color);
}

void PrimitiveBatch::DrawBox(
    const Vector3& position,
    const Vector3& scale,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawBox(position, scale, color);
}

void PrimitiveBatch::DrawBox(
    const Vector3& position,
    const Vector3& scale,
    const Vector3& originPivot,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawBox(position, scale, originPivot, color);
}

void PrimitiveBatch::DrawCircle(
    const Vector2& position,
    float radius,
    int segments,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawCircle(position, radius, segments, color);
}

void PrimitiveBatch::DrawCircle(
    const Vector3& position,
    float radius,
    int segments,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawCircle(position, radius, segments, color);
}

void PrimitiveBatch::DrawLine(
    const Vector2& start,
    const Vector2& end,
    const Color& color,
    float weight)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawLine(start, end, color, weight);
}

void PrimitiveBatch::DrawLine(
    const Matrix3x2& matrix,
    const Vector2& start,
    const Vector2& end,
    const Color& color,
    float weight)
{
    POMDOG_ASSERT(impl);
    auto transformedStart = math::Transform(start, matrix);
    auto transformedEnd = math::Transform(end, matrix);
    impl->polygonShapes.DrawLine(transformedStart, transformedEnd, color, weight);
}

void PrimitiveBatch::DrawLine(
    const Vector2& start,
    const Vector2& end,
    const Color& startColor,
    const Color& endColor,
    float weight)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawLine(start, end, startColor, endColor, weight);
}

void PrimitiveBatch::DrawPolyline(
    const std::vector<Vector2>& points,
    float thickness,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawPolyline(points, thickness, color);
}

void PrimitiveBatch::DrawRectangle(
    const Rectangle& sourceRect,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawRectangle(sourceRect, color);
}

void PrimitiveBatch::DrawRectangle(
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

void PrimitiveBatch::DrawRectangle(
    const Matrix3x2& matrix,
    const Vector2& position,
    float width,
    float height,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawRectangle(matrix, position, width, height, color);
}

void PrimitiveBatch::DrawRectangle(
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

void PrimitiveBatch::DrawRectangle(
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

void PrimitiveBatch::DrawSphere(
    const Vector3& position,
    float radius,
    const Color& color,
    int segments)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawSphere(
        position, radius, color, segments);
}

void PrimitiveBatch::DrawTriangle(
    const Vector2& point1,
    const Vector2& point2,
    const Vector2& point3,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.DrawTriangle(point1, point2, point3, color);
}

void PrimitiveBatch::DrawTriangle(
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

void PrimitiveBatch::DrawTriangle(
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

void PrimitiveBatch::DrawTriangle(
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

void PrimitiveBatch::Flush()
{
    POMDOG_ASSERT(impl);
    impl->Flush();
}

std::size_t PrimitiveBatch::GetMaxVertexCount() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->polygonShapes.GetMaxVertexCount();
}

int PrimitiveBatch::GetDrawCallCount() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->drawCallCount;
}

} // namespace pomdog
