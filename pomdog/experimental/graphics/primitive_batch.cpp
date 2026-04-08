// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/shader_loader.h"
#include "pomdog/experimental/graphics/polygon_shape_builder.h"
#include "pomdog/gpu/blend_desc.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/depth_stencil_desc.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/input_layout_builder.h"
#include "pomdog/gpu/pipeline_desc.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/rasterizer_desc.h"
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
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

class PrimitivePipelineImpl final : public PrimitivePipeline {
public:
    std::shared_ptr<gpu::PipelineState> pipelineState_;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<gpu::DepthStencilDesc>&& depthStencilDesc,
        std::optional<gpu::RasterizerDesc>&& rasterizerDesc);
};

std::unique_ptr<Error>
PrimitivePipelineImpl::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::DepthStencilDesc>&& depthStencilDesc,
    std::optional<gpu::RasterizerDesc>&& rasterizerDesc)
{
    if (!depthStencilDesc) {
        depthStencilDesc = gpu::DepthStencilDesc::createNone();
    }
    if (!rasterizerDesc) {
        rasterizerDesc = gpu::RasterizerDesc::createCullCounterClockwise();
    }

    POMDOG_ASSERT(depthStencilDesc);
    POMDOG_ASSERT(rasterizerDesc);

    {
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

        gpu::PipelineDesc pipelineDesc = {};
        pipelineDesc.renderTargetViewFormats = {presentationParameters.backBufferFormat};
        pipelineDesc.depthStencilViewFormat = presentationParameters.depthStencilFormat;
        pipelineDesc.vertexShader = std::move(vertexShader);
        pipelineDesc.pixelShader = std::move(pixelShader);
        gpu::InputLayoutBuilder::addVertex(pipelineDesc.inputLayout,
            0, gpu::InputClassification::PerVertex, 16,
            {
                gpu::InputElementFormat::Float32x3,
                gpu::InputElementFormat::Unorm8x4,
            });
        pipelineDesc.primitiveTopology = gpu::PrimitiveTopology::TriangleList;
        pipelineDesc.blendState = gpu::BlendDesc::createNonPremultiplied();
        pipelineDesc.depthStencilState = *depthStencilDesc;
        pipelineDesc.rasterizerState = *rasterizerDesc;
        pipelineDesc.multiSampleMask = std::numeric_limits<u32>::max();

        auto [pipeline, pipelineErr] = graphicsDevice->createPipelineState(pipelineDesc);
        if (pipelineErr != nullptr) {
            return errors::wrap(std::move(pipelineErr), "failed to create pipeline state");
        }
        pipelineState_ = std::move(pipeline);
    }

    return nullptr;
}

class PrimitiveBatchImpl final : public PrimitiveBatch {
public:
    using Vertex = PrimitiveBatchVertex;

private:
    std::vector<Vertex> vertices_;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer_;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer_;
    PolygonShapeBuilder polygonShapes_;
    u32 maxVertexCount_ = 0;
    u32 nextVertex_ = 0;
    u32 drawCallCount_ = 0;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<u32> batchSize);

    void reset() override;

    void setTransform(const Matrix4x4& transformMatrix) override;

    void drawArc(
        const Vector2& position, f32 radius,
        const Radian<f32>& startAngle, const Radian<f32>& arcAngle,
        i32 segments, const Color& color) override;

    void drawBox(const BoundingBox& box, const Color& color) override;
    void drawBox(const Vector3& position, const Vector3& scale, const Color& color) override;
    void drawBox(const Vector3& position, const Vector3& scale, const Vector3& originPivot, const Color& color) override;

    void drawCircle(const Vector2& position, f32 radius, i32 segments, const Color& color) override;
    void drawCircle(const Vector3& position, f32 radius, i32 segments, const Color& color) override;

    void drawLine(const Vector2& start, const Vector2& end, const Color& color, f32 weight) override;
    void drawLine(const Matrix3x2& matrix, const Vector2& start, const Vector2& end, const Color& color, f32 weight) override;
    void drawLine(const Vector2& start, const Vector2& end, const Color& startColor, const Color& endColor, f32 weight) override;

    void drawPolyline(const std::vector<Vector2>& points, f32 thickness, const Color& color) override;

    void drawRectangle(const Rect2D& sourceRect, const Color& color) override;
    void drawRectangle(const Rect2D& sourceRect, const Color& color1, const Color& color2, const Color& color3, const Color& color4) override;
    void drawRectangle(const Matrix3x2& matrix, const Vector2& position, f32 width, f32 height, const Color& color) override;
    void drawRectangle(const Matrix3x2& matrix, const Vector2& position, f32 width, f32 height, const Color& color1, const Color& color2, const Color& color3, const Color& color4) override;
    void drawRectangle(const Vector2& position, f32 width, f32 height, const Vector2& originPivot, const Color& color) override;

    void drawSphere(const Vector3& position, f32 radius, const Color& color, i32 segments) override;

    void drawTriangle(const Vector2& point1, const Vector2& point2, const Vector2& point3, const Color& color) override;
    void drawTriangle(const Vector2& point1, const Vector2& point2, const Vector2& point3, const Color& color1, const Color& color2, const Color& color3) override;
    void drawTriangle(const Vector3& point1, const Vector3& point2, const Vector3& point3, const Color& color1, const Color& color2, const Color& color3) override;

    void flush(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<PrimitivePipeline>& primitivePipeline) override;
    void submit(const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) override;

    [[nodiscard]] u32 getMaxVertexCount() const noexcept override;
    [[nodiscard]] u32 getDrawCallCount() const noexcept override;
};

std::unique_ptr<Error>
PrimitiveBatchImpl::initialize(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize)
{
    static constexpr u32 DefaultMaxVertexCount = 4096;
    static constexpr u32 MinVertexCount = 64;

    maxVertexCount_ = batchSize.value_or(DefaultMaxVertexCount);

    if (maxVertexCount_ < MinVertexCount) {
        return errors::make("batch size must be at least " + std::to_string(MinVertexCount));
    }

    vertices_.reserve(MinVertexCount);

    {
        if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
                maxVertexCount_,
                sizeof(Vertex),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
        else {
            vertexBuffer_ = std::move(buffer);
        }
    }

    if (auto [buffer, err] = graphicsDevice->createConstantBuffer(
            sizeof(Matrix4x4),
            gpu::BufferUsage::Dynamic);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create constant buffer");
    }
    else {
        constantBuffer_ = std::move(buffer);
    }

    return nullptr;
}

void PrimitiveBatchImpl::reset()
{
    vertices_.clear();
    polygonShapes_.reset();
    nextVertex_ = 0;
    drawCallCount_ = 0;
}

void PrimitiveBatchImpl::setTransform(const Matrix4x4& transformMatrix)
{
    alignas(16) Matrix4x4 transposedMatrix = math::transpose(transformMatrix);
    constantBuffer_->setData(0, gpu::makeByteSpan(transposedMatrix));
}

void PrimitiveBatchImpl::flush(
    const std::shared_ptr<gpu::CommandList>& commandList,
    const std::shared_ptr<PrimitivePipeline>& primitivePipeline)
{
    if (polygonShapes_.isEmpty()) {
        return;
    }

    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(primitivePipeline);

    const auto vertexCount = polygonShapes_.getVertexCount();

    // NOTE: Accumulate vertex data for later upload in submit()
    const auto* data = polygonShapes_.getData();
    vertices_.insert(vertices_.end(), data, data + vertexCount);

    auto* pipeline = static_cast<PrimitivePipelineImpl*>(primitivePipeline.get());

    commandList->setVertexBuffer(0, vertexBuffer_);
    commandList->setPipelineState(pipeline->pipelineState_);
    commandList->setConstantBuffer(0, constantBuffer_);
    commandList->draw(vertexCount, nextVertex_);

    nextVertex_ += vertexCount;
    ++drawCallCount_;

    polygonShapes_.reset();
}

void PrimitiveBatchImpl::submit(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    if (vertices_.empty()) {
        return;
    }

    const auto vertexCount = static_cast<u32>(vertices_.size());

    if (vertexCount > maxVertexCount_) {
        // NOTE: Grow the vertex buffer
        auto requiredVertexCount = vertexCount + 2048;
        if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
                requiredVertexCount,
                sizeof(Vertex),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return;
        }
        else {
            vertexBuffer_ = std::move(buffer);
            maxVertexCount_ = requiredVertexCount;
        }
    }

    vertexBuffer_->setData(
        0,
        vertices_.data(),
        vertexCount,
        sizeof(Vertex));

    // NOTE: Shrink if over-allocated
    if (vertices_.capacity() > maxVertexCount_ * 2) {
        vertices_.resize(maxVertexCount_);
        vertices_.shrink_to_fit();
    }
}

void PrimitiveBatchImpl::drawArc(
    const Vector2& position,
    f32 radius,
    const Radian<f32>& startAngle,
    const Radian<f32>& arcAngle,
    i32 segments,
    const Color& color)
{
    polygonShapes_.drawArc(
        position, radius, startAngle, arcAngle, segments, color);
}

void PrimitiveBatchImpl::drawBox(
    const BoundingBox& box,
    const Color& color)
{
    polygonShapes_.drawBox(box.min, box.max - box.min, color);
}

void PrimitiveBatchImpl::drawBox(
    const Vector3& position,
    const Vector3& scale,
    const Color& color)
{
    polygonShapes_.drawBox(position, scale, color);
}

void PrimitiveBatchImpl::drawBox(
    const Vector3& position,
    const Vector3& scale,
    const Vector3& originPivot,
    const Color& color)
{
    polygonShapes_.drawBox(position, scale, originPivot, color);
}

void PrimitiveBatchImpl::drawCircle(
    const Vector2& position,
    f32 radius,
    i32 segments,
    const Color& color)
{
    polygonShapes_.drawCircle(position, radius, segments, color);
}

void PrimitiveBatchImpl::drawCircle(
    const Vector3& position,
    f32 radius,
    i32 segments,
    const Color& color)
{
    polygonShapes_.drawCircle(position, radius, segments, color);
}

void PrimitiveBatchImpl::drawLine(
    const Vector2& start,
    const Vector2& end,
    const Color& color,
    f32 weight)
{
    polygonShapes_.drawLine(start, end, color, weight);
}

void PrimitiveBatchImpl::drawLine(
    const Matrix3x2& matrix,
    const Vector2& start,
    const Vector2& end,
    const Color& color,
    f32 weight)
{
    auto transformedStart = math::transform(start, matrix);
    auto transformedEnd = math::transform(end, matrix);
    polygonShapes_.drawLine(transformedStart, transformedEnd, color, weight);
}

void PrimitiveBatchImpl::drawLine(
    const Vector2& start,
    const Vector2& end,
    const Color& startColor,
    const Color& endColor,
    f32 weight)
{
    polygonShapes_.drawLine(start, end, startColor, endColor, weight);
}

void PrimitiveBatchImpl::drawPolyline(
    const std::vector<Vector2>& points,
    f32 thickness,
    const Color& color)
{
    polygonShapes_.drawPolyline(points, thickness, color);
}

void PrimitiveBatchImpl::drawRectangle(
    const Rect2D& sourceRect,
    const Color& color)
{
    polygonShapes_.drawRectangle(sourceRect, color);
}

void PrimitiveBatchImpl::drawRectangle(
    const Rect2D& sourceRect,
    const Color& color1,
    const Color& color2,
    const Color& color3,
    const Color& color4)
{
    polygonShapes_.drawRectangle(
        sourceRect, color1, color2, color3, color4);
}

void PrimitiveBatchImpl::drawRectangle(
    const Matrix3x2& matrix,
    const Vector2& position,
    f32 width,
    f32 height,
    const Color& color)
{
    polygonShapes_.drawRectangle(matrix, position, width, height, color);
}

void PrimitiveBatchImpl::drawRectangle(
    const Matrix3x2& matrix,
    const Vector2& position,
    f32 width,
    f32 height,
    const Color& color1,
    const Color& color2,
    const Color& color3,
    const Color& color4)
{
    polygonShapes_.drawRectangle(
        matrix, position, width, height, color1, color2, color3, color4);
}

void PrimitiveBatchImpl::drawRectangle(
    const Vector2& position,
    f32 width,
    f32 height,
    const Vector2& originPivot,
    const Color& color)
{
    polygonShapes_.drawRectangle(
        position, width, height, originPivot, color);
}

void PrimitiveBatchImpl::drawSphere(
    const Vector3& position,
    f32 radius,
    const Color& color,
    i32 segments)
{
    polygonShapes_.drawSphere(
        position, radius, color, segments);
}

void PrimitiveBatchImpl::drawTriangle(
    const Vector2& point1,
    const Vector2& point2,
    const Vector2& point3,
    const Color& color)
{
    polygonShapes_.drawTriangle(point1, point2, point3, color);
}

void PrimitiveBatchImpl::drawTriangle(
    const Vector2& point1,
    const Vector2& point2,
    const Vector2& point3,
    const Color& color1,
    const Color& color2,
    const Color& color3)
{
    polygonShapes_.drawTriangle(
        point1, point2, point3, color1, color2, color3);
}

void PrimitiveBatchImpl::drawTriangle(
    const Vector3& point1,
    const Vector3& point2,
    const Vector3& point3,
    const Color& color1,
    const Color& color2,
    const Color& color3)
{
    polygonShapes_.drawTriangle(
        point1, point2, point3, color1, color2, color3);
}

u32 PrimitiveBatchImpl::getMaxVertexCount() const noexcept
{
    return maxVertexCount_;
}

u32 PrimitiveBatchImpl::getDrawCallCount() const noexcept
{
    return drawCallCount_;
}

} // namespace

PrimitivePipeline::~PrimitivePipeline() = default;

PrimitiveBatch::~PrimitiveBatch() = default;

std::tuple<std::shared_ptr<PrimitivePipeline>, std::unique_ptr<Error>>
createPrimitivePipeline(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) noexcept
{
    return createPrimitivePipeline(fs, graphicsDevice, std::nullopt, std::nullopt);
}

std::tuple<std::shared_ptr<PrimitivePipeline>, std::unique_ptr<Error>>
createPrimitivePipeline(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::DepthStencilDesc>&& depthStencilDesc,
    std::optional<gpu::RasterizerDesc>&& rasterizerDesc) noexcept
{
    auto pipeline = std::make_shared<PrimitivePipelineImpl>();
    if (auto err = pipeline->initialize(fs, graphicsDevice, std::move(depthStencilDesc), std::move(rasterizerDesc)); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(pipeline), nullptr);
}

std::tuple<std::shared_ptr<PrimitiveBatch>, std::unique_ptr<Error>>
createPrimitiveBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize) noexcept
{
    auto primitiveBatch = std::make_shared<PrimitiveBatchImpl>();
    if (auto err = primitiveBatch->initialize(graphicsDevice, batchSize); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(primitiveBatch), nullptr);
}

} // namespace pomdog
