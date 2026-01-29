// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/asset_builders/shader_builder.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/experimental/graphics/polygon_shape_builder.h"
#include "pomdog/gpu/blend_descriptor.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/depth_stencil_descriptor.h"
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
#include "pomdog/math/math_constants.h"
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

    void begin(
        const std::shared_ptr<gpu::CommandList>& commandListIn,
        const Matrix4x4& transformMatrix);

    void drawTriangle(
        const Vector2& point1, const Vector2& point2, const Vector2& point3,
        const Vector4& color1, const Vector4& color2, const Vector4& color3);

    void end();

    void flush();
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
        depthStencilDesc = gpu::DepthStencilDescriptor::createNone();
    }
    if (!rasterizerDesc) {
        rasterizerDesc = gpu::RasterizerDescriptor::createCullCounterClockwise();
    }

    POMDOG_ASSERT(depthStencilDesc);
    POMDOG_ASSERT(rasterizerDesc);

    {
        auto maxVertexCount = polygonShapes.getMaxVertexCount();
        vertexBuffer = std::get<0>(graphicsDevice->createVertexBuffer(
            maxVertexCount,
            sizeof(Vertex),
            gpu::BufferUsage::Dynamic));
    }
    {
        auto inputLayout = gpu::InputLayoutHelper{}
                               .addFloat3()
                               .addFloat4();

        auto [vertexShader, vertexShaderErr] = assets.createBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader)
                                                   .setGLSL(Builtin_GLSL_PrimitiveBatch_VS, std::strlen(Builtin_GLSL_PrimitiveBatch_VS))
                                                   .setHLSLPrecompiled(BuiltinHLSL_PrimitiveBatch_VS, sizeof(BuiltinHLSL_PrimitiveBatch_VS))
                                                   .setMetal(Builtin_Metal_PrimitiveBatch, std::strlen(Builtin_Metal_PrimitiveBatch), "PrimitiveBatchVS")
                                                   .build();

        if (vertexShaderErr != nullptr) {
            // FIXME: error handling
        }

        auto [pixelShader, pixelShaderErr] = assets.createBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader)
                                                 .setGLSL(Builtin_GLSL_PrimitiveBatch_PS, std::strlen(Builtin_GLSL_PrimitiveBatch_PS))
                                                 .setHLSLPrecompiled(BuiltinHLSL_PrimitiveBatch_PS, sizeof(BuiltinHLSL_PrimitiveBatch_PS))
                                                 .setMetal(Builtin_Metal_PrimitiveBatch, std::strlen(Builtin_Metal_PrimitiveBatch), "PrimitiveBatchPS")
                                                 .build();

        if (pixelShaderErr != nullptr) {
            // FIXME: error handling
        }

        auto presentationParameters = graphicsDevice->getPresentationParameters();

        std::unique_ptr<Error> pipelineStateErr;
        std::tie(pipelineState, pipelineStateErr) = assets.createBuilder<gpu::PipelineState>()
                                                        .setRenderTargetViewFormat(presentationParameters.backBufferFormat)
                                                        .setDepthStencilViewFormat(presentationParameters.depthStencilFormat)
                                                        .setVertexShader(std::move(vertexShader))
                                                        .setPixelShader(std::move(pixelShader))
                                                        .setInputLayout(inputLayout.createInputLayout())
                                                        .setPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
                                                        .setBlendState(gpu::BlendDescriptor::createNonPremultiplied())
                                                        .setDepthStencilState(*depthStencilDesc)
                                                        .setRasterizerState(*rasterizerDesc)
                                                        .setConstantBufferBindSlot("TransformMatrix", 0)
                                                        .build();

        if (pipelineStateErr != nullptr) {
            // FIXME: error handling
        }
    }

    constantBuffer = std::get<0>(graphicsDevice->createConstantBuffer(
        sizeof(Matrix4x4),
        gpu::BufferUsage::Dynamic));
}

void PrimitiveBatch::Impl::begin(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const Matrix4x4& transformMatrix)
{
    POMDOG_ASSERT(commandListIn);
    commandList = commandListIn;

    alignas(16) Matrix4x4 transposedMatrix = math::transpose(transformMatrix);
    constantBuffer->setData(0, gpu::makeByteSpan(transposedMatrix));

    startVertexLocation = 0;
    drawCallCount = 0;
}

void PrimitiveBatch::Impl::end()
{
    flush();
    commandList.reset();
}

void PrimitiveBatch::Impl::flush()
{
    if (polygonShapes.isEmpty()) {
        return;
    }

    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(!polygonShapes.isEmpty());
    POMDOG_ASSERT((startVertexLocation + polygonShapes.getVertexCount()) <= polygonShapes.getMaxVertexCount());

    const auto vertexOffsetBytes = sizeof(Vertex) * startVertexLocation;
    vertexBuffer->setData(
        vertexOffsetBytes,
        polygonShapes.getData(),
        polygonShapes.getVertexCount(),
        sizeof(Vertex));

    commandList->setVertexBuffer(0, vertexBuffer);
    commandList->setPipelineState(pipelineState);
    commandList->setConstantBuffer(0, constantBuffer);
    commandList->draw(polygonShapes.getVertexCount(), startVertexLocation);

    startVertexLocation += polygonShapes.getVertexCount();
    ++drawCallCount;

    polygonShapes.reset();
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

void PrimitiveBatch::begin(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const Matrix4x4& transformMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->begin(commandListIn, transformMatrixIn);
}

void PrimitiveBatch::end()
{
    POMDOG_ASSERT(impl);
    impl->end();
}

void PrimitiveBatch::drawArc(
    const Vector2& position,
    float radius,
    const Radian<float>& startAngle,
    const Radian<float>& arcAngle,
    int segments,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawArc(
        position, radius, startAngle, arcAngle, segments, color);
}

void PrimitiveBatch::drawBox(
    const BoundingBox& box,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawBox(box.min, box.max - box.min, color);
}

void PrimitiveBatch::drawBox(
    const Vector3& position,
    const Vector3& scale,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawBox(position, scale, color);
}

void PrimitiveBatch::drawBox(
    const Vector3& position,
    const Vector3& scale,
    const Vector3& originPivot,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawBox(position, scale, originPivot, color);
}

void PrimitiveBatch::drawCircle(
    const Vector2& position,
    float radius,
    int segments,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawCircle(position, radius, segments, color);
}

void PrimitiveBatch::drawCircle(
    const Vector3& position,
    float radius,
    int segments,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawCircle(position, radius, segments, color);
}

void PrimitiveBatch::drawLine(
    const Vector2& start,
    const Vector2& end,
    const Color& color,
    float weight)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawLine(start, end, color, weight);
}

void PrimitiveBatch::drawLine(
    const Matrix3x2& matrix,
    const Vector2& start,
    const Vector2& end,
    const Color& color,
    float weight)
{
    POMDOG_ASSERT(impl);
    auto transformedStart = math::transform(start, matrix);
    auto transformedEnd = math::transform(end, matrix);
    impl->polygonShapes.drawLine(transformedStart, transformedEnd, color, weight);
}

void PrimitiveBatch::drawLine(
    const Vector2& start,
    const Vector2& end,
    const Color& startColor,
    const Color& endColor,
    float weight)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawLine(start, end, startColor, endColor, weight);
}

void PrimitiveBatch::drawPolyline(
    const std::vector<Vector2>& points,
    float thickness,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawPolyline(points, thickness, color);
}

void PrimitiveBatch::drawRectangle(
    const Rectangle& sourceRect,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawRectangle(sourceRect, color);
}

void PrimitiveBatch::drawRectangle(
    const Rectangle& sourceRect,
    const Color& color1,
    const Color& color2,
    const Color& color3,
    const Color& color4)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawRectangle(
        sourceRect, color1, color2, color3, color4);
}

void PrimitiveBatch::drawRectangle(
    const Matrix3x2& matrix,
    const Vector2& position,
    float width,
    float height,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawRectangle(matrix, position, width, height, color);
}

void PrimitiveBatch::drawRectangle(
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
    impl->polygonShapes.drawRectangle(
        matrix, position, width, height, color1, color2, color3, color4);
}

void PrimitiveBatch::drawRectangle(
    const Vector2& position,
    float width,
    float height,
    const Vector2& originPivot,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawRectangle(
        position, width, height, originPivot, color);
}

void PrimitiveBatch::drawSphere(
    const Vector3& position,
    float radius,
    const Color& color,
    int segments)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawSphere(
        position, radius, color, segments);
}

void PrimitiveBatch::drawTriangle(
    const Vector2& point1,
    const Vector2& point2,
    const Vector2& point3,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawTriangle(point1, point2, point3, color);
}

void PrimitiveBatch::drawTriangle(
    const Vector2& point1,
    const Vector2& point2,
    const Vector2& point3,
    const Color& color1,
    const Color& color2,
    const Color& color3)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawTriangle(
        point1, point2, point3, color1, color2, color3);
}

void PrimitiveBatch::drawTriangle(
    const Vector3& point1,
    const Vector3& point2,
    const Vector3& point3,
    const Color& color1,
    const Color& color2,
    const Color& color3)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawTriangle(
        point1, point2, point3, color1, color2, color3);
}

void PrimitiveBatch::drawTriangle(
    const Vector3& point1,
    const Vector3& point2,
    const Vector3& point3,
    const Vector4& color1,
    const Vector4& color2,
    const Vector4& color3)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawTriangle(
        point1, point2, point3, color1, color2, color3);
}

void PrimitiveBatch::flush()
{
    POMDOG_ASSERT(impl);
    impl->flush();
}

std::size_t PrimitiveBatch::getMaxVertexCount() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->polygonShapes.getMaxVertexCount();
}

int PrimitiveBatch::getDrawCallCount() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->drawCallCount;
}

} // namespace pomdog
