// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/shader_loader.h"
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
#include "pomdog/gpu/shader_pipeline_stage.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/math/bounding_box.h"
#include "pomdog/math/color.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/matrix3x2.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

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
    u32 startVertexLocation_ = 0;
    u32 drawCallCount_ = 0;

public:
    Impl() = default;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<gpu::DepthStencilDescriptor>&& depthStencilDesc,
        std::optional<gpu::RasterizerDescriptor>&& rasterizerDesc);

    void begin(
        const std::shared_ptr<gpu::CommandList>& commandListIn,
        const Matrix4x4& transformMatrix);

    void drawTriangle(
        const Vector2& point1, const Vector2& point2, const Vector2& point3,
        const Vector4& color1, const Vector4& color2, const Vector4& color3);

    void end();

    void flush();
};

std::unique_ptr<Error>
PrimitiveBatch::Impl::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::DepthStencilDescriptor>&& depthStencilDesc,
    std::optional<gpu::RasterizerDescriptor>&& rasterizerDesc)
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
        if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
                maxVertexCount,
                sizeof(Vertex),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
        else {
            vertexBuffer = std::move(buffer);
        }
    }
    {
        auto inputLayout = gpu::InputLayoutHelper{}
                               .addFloat3()
                               .addFloat4();

        auto [vertexShader, vsErr] = loadShaderAutomagically(
            fs, graphicsDevice,
            gpu::ShaderPipelineStage::VertexShader,
            "/assets/shaders", "primitive_batch_vs", "primitive_batch_vs");
        if (vsErr != nullptr) {
            return errors::wrap(std::move(vsErr), "failed to load vertex shader");
        }

        auto [pixelShader, psErr] = loadShaderAutomagically(
            fs, graphicsDevice,
            gpu::ShaderPipelineStage::PixelShader,
            "/assets/shaders", "primitive_batch_ps", "primitive_batch_ps");
        if (psErr != nullptr) {
            return errors::wrap(std::move(psErr), "failed to load pixel shader");
        }

        auto presentationParameters = graphicsDevice->getPresentationParameters();

        auto pipelineStateBuilder = PipelineStateBuilder(graphicsDevice);
        pipelineStateBuilder.setRenderTargetViewFormat(presentationParameters.backBufferFormat);
        pipelineStateBuilder.setDepthStencilViewFormat(presentationParameters.depthStencilFormat);
        pipelineStateBuilder.setVertexShader(std::move(vertexShader));
        pipelineStateBuilder.setPixelShader(std::move(pixelShader));
        pipelineStateBuilder.setInputLayout(inputLayout.createInputLayout());
        pipelineStateBuilder.setPrimitiveTopology(gpu::PrimitiveTopology::TriangleList);
        pipelineStateBuilder.setBlendState(gpu::BlendDescriptor::createNonPremultiplied());
        pipelineStateBuilder.setDepthStencilState(*depthStencilDesc);
        pipelineStateBuilder.setRasterizerState(*rasterizerDesc);

        auto [pipeline, pipelineErr] = pipelineStateBuilder.build();
        if (pipelineErr != nullptr) {
            return errors::wrap(std::move(pipelineErr), "failed to create pipeline state");
        }
        pipelineState = std::move(pipeline);
    }

    if (auto [buffer, err] = graphicsDevice->createConstantBuffer(
            sizeof(Matrix4x4),
            gpu::BufferUsage::Dynamic);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create constant buffer");
    }
    else {
        constantBuffer = std::move(buffer);
    }

    return nullptr;
}

void PrimitiveBatch::Impl::begin(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const Matrix4x4& transformMatrix)
{
    POMDOG_ASSERT(commandListIn);
    commandList = commandListIn;

    alignas(16) Matrix4x4 transposedMatrix = math::transpose(transformMatrix);
    constantBuffer->setData(0, gpu::makeByteSpan(transposedMatrix));

    startVertexLocation_ = 0;
    drawCallCount_ = 0;
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
    POMDOG_ASSERT((startVertexLocation_ + polygonShapes.getVertexCount()) <= polygonShapes.getMaxVertexCount());

    const auto vertexOffsetBytes = static_cast<u32>(sizeof(Vertex) * startVertexLocation_);
    vertexBuffer->setData(
        vertexOffsetBytes,
        polygonShapes.getData(),
        static_cast<u32>(polygonShapes.getVertexCount()),
        sizeof(Vertex));

    commandList->setVertexBuffer(0, vertexBuffer);
    commandList->setPipelineState(pipelineState);
    commandList->setConstantBuffer(0, constantBuffer);
    commandList->draw(polygonShapes.getVertexCount(), startVertexLocation_);

    startVertexLocation_ += polygonShapes.getVertexCount();
    ++drawCallCount_;

    polygonShapes.reset();
}

// MARK: - PrimitiveBatch

PrimitiveBatch::PrimitiveBatch() = default;

PrimitiveBatch::~PrimitiveBatch() = default;

std::unique_ptr<Error>
PrimitiveBatch::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    return initialize(fs, graphicsDevice, std::nullopt, std::nullopt);
}

std::unique_ptr<Error>
PrimitiveBatch::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::DepthStencilDescriptor>&& depthStencilDesc,
    std::optional<gpu::RasterizerDescriptor>&& rasterizerDesc)
{
    impl = std::make_unique<Impl>();
    return impl->initialize(
        fs,
        graphicsDevice,
        std::move(depthStencilDesc),
        std::move(rasterizerDesc));
}

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
    f32 radius,
    const Radian<f32>& startAngle,
    const Radian<f32>& arcAngle,
    i32 segments,
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
    f32 radius,
    i32 segments,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawCircle(position, radius, segments, color);
}

void PrimitiveBatch::drawCircle(
    const Vector3& position,
    f32 radius,
    i32 segments,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawCircle(position, radius, segments, color);
}

void PrimitiveBatch::drawLine(
    const Vector2& start,
    const Vector2& end,
    const Color& color,
    f32 weight)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawLine(start, end, color, weight);
}

void PrimitiveBatch::drawLine(
    const Matrix3x2& matrix,
    const Vector2& start,
    const Vector2& end,
    const Color& color,
    f32 weight)
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
    f32 weight)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawLine(start, end, startColor, endColor, weight);
}

void PrimitiveBatch::drawPolyline(
    const std::vector<Vector2>& points,
    f32 thickness,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawPolyline(points, thickness, color);
}

void PrimitiveBatch::drawRectangle(
    const Rect2D& sourceRect,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawRectangle(sourceRect, color);
}

void PrimitiveBatch::drawRectangle(
    const Rect2D& sourceRect,
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
    f32 width,
    f32 height,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawRectangle(matrix, position, width, height, color);
}

void PrimitiveBatch::drawRectangle(
    const Matrix3x2& matrix,
    const Vector2& position,
    f32 width,
    f32 height,
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
    f32 width,
    f32 height,
    const Vector2& originPivot,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->polygonShapes.drawRectangle(
        position, width, height, originPivot, color);
}

void PrimitiveBatch::drawSphere(
    const Vector3& position,
    f32 radius,
    const Color& color,
    i32 segments)
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

u32 PrimitiveBatch::getMaxVertexCount() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->polygonShapes.getMaxVertexCount();
}

u32 PrimitiveBatch::getDrawCallCount() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->drawCallCount_;
}

} // namespace pomdog
