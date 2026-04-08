// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/line_batch.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/shader_loader.h"
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
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_pipeline_stage.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/math/bounding_box.h"
#include "pomdog/math/color.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/matrix3x2.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

class LinePipelineImpl final : public LinePipeline {
public:
    std::shared_ptr<gpu::PipelineState> pipelineState_;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);
};

std::unique_ptr<Error>
LinePipelineImpl::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    auto [vertexShader, vsErr] = loadShaderAutomagically(
        fs, graphicsDevice,
        gpu::ShaderPipelineStage::VertexShader,
        "/assets/shaders", "line_batch_vs", "line_batch_vs");
    if (vsErr != nullptr) {
        return errors::wrap(std::move(vsErr), "failed to load vertex shader");
    }

    auto [pixelShader, psErr] = loadShaderAutomagically(
        fs, graphicsDevice,
        gpu::ShaderPipelineStage::PixelShader,
        "/assets/shaders", "line_batch_ps", "line_batch_ps");
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
    pipelineDesc.primitiveTopology = gpu::PrimitiveTopology::LineList;
    pipelineDesc.blendState = gpu::BlendDesc::createNonPremultiplied();
    pipelineDesc.depthStencilState = gpu::DepthStencilDesc::createDefault();
    pipelineDesc.rasterizerState = gpu::RasterizerDesc::createDefault();
    pipelineDesc.multiSampleMask = std::numeric_limits<u32>::max();

    auto [pipeline, pipelineErr] = graphicsDevice->createPipelineState(pipelineDesc);
    if (pipelineErr != nullptr) {
        return errors::wrap(std::move(pipelineErr), "failed to create pipeline state");
    }
    pipelineState_ = std::move(pipeline);

    return nullptr;
}

class LineBatchImpl final : public LineBatch {
public:
    static constexpr u32 DefaultMaxVertexCount = 4096;
    static constexpr u32 MinVertexCount = 64;

    struct Vertex final {
        // {xyz} = position.xyz
        Vector3 position;

        // {rgba} = color
        Color color;
    };

private:
    std::vector<Vertex> vertices_;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer_;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer_;
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

    void drawBox(const BoundingBox& box, const Color& color) override;
    void drawBox(const Vector3& position, const Vector3& scale, const Color& color) override;
    void drawBox(const Vector3& position, const Vector3& scale, const Vector3& originPivot, const Color& color) override;

    void drawCircle(const Vector2& position, f32 radius, const Color& color, i32 segments) override;

    void drawLine(const Vector2& start, const Vector2& end, const Color& color) override;
    void drawLine(const Vector2& start, const Vector2& end, const Color& startColor, const Color& endColor) override;
    void drawLine(const Vector3& start, const Vector3& end, const Color& color) override;
    void drawLine(const Vector3& start, const Vector3& end, const Color& startColor, const Color& endColor) override;

    void drawRectangle(const Rect2D& sourceRect, const Color& color) override;
    void drawRectangle(const Rect2D& sourceRect, const Color& color1, const Color& color2, const Color& color3, const Color& color4) override;
    void drawRectangle(const Matrix3x2& matrix, const Rect2D& sourceRect, const Color& color) override;

    void drawSphere(const Vector3& position, f32 radius, const Color& color, i32 segments) override;

    void drawTriangle(const Vector2& point1, const Vector2& point2, const Vector2& point3, const Color& color) override;
    void drawTriangle(const Vector2& point1, const Vector2& point2, const Vector2& point3, const Color& color1, const Color& color2, const Color& color3) override;

    void flush(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<LinePipeline>& linePipeline) override;
    void submit(const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) override;
    [[nodiscard]] u32 getDrawCallCount() const noexcept override;

private:
    void drawLineImpl(
        const Vector2& point1, const Vector2& point2,
        const Color& color1, const Color& color2);

    void drawLineImpl(
        const Vector3& point1, const Vector3& point2,
        const Color& color1, const Color& color2);

    void drawTriangleImpl(
        const Vector2& point1, const Vector2& point2, const Vector2& point3,
        const Color& color1, const Color& color2, const Color& color3);
};

std::unique_ptr<Error>
LineBatchImpl::initialize(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize)
{
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

void LineBatchImpl::reset()
{
    vertices_.clear();
    nextVertex_ = 0;
    drawCallCount_ = 0;
}

void LineBatchImpl::setTransform(const Matrix4x4& transformMatrix)
{
    alignas(16) Matrix4x4 transposedMatrix = math::transpose(transformMatrix);
    constantBuffer_->setData(0, gpu::makeByteSpan(transposedMatrix));
}

void LineBatchImpl::flush(
    const std::shared_ptr<gpu::CommandList>& commandList,
    const std::shared_ptr<LinePipeline>& linePipeline)
{
    if (vertices_.empty()) {
        return;
    }

    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(linePipeline);

    auto* pipeline = static_cast<LinePipelineImpl*>(linePipeline.get());

    const auto vertexCount = static_cast<u32>(vertices_.size());

    commandList->setVertexBuffer(0, vertexBuffer_);
    commandList->setPipelineState(pipeline->pipelineState_);
    commandList->setConstantBuffer(0, constantBuffer_);
    commandList->draw(vertexCount, 0);

    ++drawCallCount_;
}

void LineBatchImpl::submit(
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

    vertexBuffer_->setData(vertices_.data(), vertexCount);

    // NOTE: Shrink if over-allocated
    if (vertices_.capacity() > maxVertexCount_ * 2) {
        vertices_.resize(maxVertexCount_);
        vertices_.shrink_to_fit();
    }
}

u32 LineBatchImpl::getDrawCallCount() const noexcept
{
    return drawCallCount_;
}

void LineBatchImpl::drawLineImpl(
    const Vector2& point1, const Vector2& point2,
    const Color& color1, const Color& color2)
{
    vertices_.push_back(Vertex{Vector3(point1, 0.0f), color1});
    vertices_.push_back(Vertex{Vector3(point2, 0.0f), color2});
}

void LineBatchImpl::drawLineImpl(
    const Vector3& point1, const Vector3& point2,
    const Color& color1, const Color& color2)
{
    vertices_.push_back(Vertex{point1, color1});
    vertices_.push_back(Vertex{point2, color2});
}

void LineBatchImpl::drawTriangleImpl(
    const Vector2& point1, const Vector2& point2, const Vector2& point3,
    const Color& color1, const Color& color2, const Color& color3)
{
    vertices_.push_back(Vertex{Vector3(point1, 0.0f), color1});
    vertices_.push_back(Vertex{Vector3(point2, 0.0f), color2});
    vertices_.push_back(Vertex{Vector3(point2, 0.0f), color2});
    vertices_.push_back(Vertex{Vector3(point3, 0.0f), color3});
    vertices_.push_back(Vertex{Vector3(point3, 0.0f), color3});
    vertices_.push_back(Vertex{Vector3(point1, 0.0f), color1});
}

void LineBatchImpl::drawBox(const BoundingBox& box, const Color& color)
{
    drawBox(box.min, box.max - box.min, Vector3::createZero(), color);
}

void LineBatchImpl::drawBox(
    const Vector3& position,
    const Vector3& scale,
    const Color& color)
{
    drawBox(position, scale, Vector3::createZero(), color);
}

void LineBatchImpl::drawBox(
    const Vector3& position,
    const Vector3& scale,
    const Vector3& originPivot,
    const Color& color)
{
    Vector3 boxVertices[] = {
        Vector3{0.0f, 0.0f, 0.0f},
        Vector3{0.0f, 0.0f, 1.0f},
        Vector3{0.0f, 1.0f, 0.0f},
        Vector3{0.0f, 1.0f, 1.0f},
        Vector3{1.0f, 0.0f, 0.0f},
        Vector3{1.0f, 0.0f, 1.0f},
        Vector3{1.0f, 1.0f, 0.0f},
        Vector3{1.0f, 1.0f, 1.0f},
    };

    for (auto& v : boxVertices) {
        v = ((v - originPivot) * scale) + position;
    }

    auto draw = [&](int a, int b) {
        drawLineImpl(
            boxVertices[a],
            boxVertices[b],
            color,
            color);
    };

    draw(0, 1);
    draw(0, 2);
    draw(2, 3);
    draw(3, 1);
    draw(4, 5);
    draw(4, 6);
    draw(6, 7);
    draw(7, 5);
    draw(0, 4);
    draw(2, 6);
    draw(3, 7);
    draw(1, 5);
}

void LineBatchImpl::drawCircle(const Vector2& position, f32 radius, const Color& color, i32 segments)
{
    POMDOG_ASSERT(segments >= 3);
    POMDOG_ASSERT(radius >= 0);

    if (radius <= 0) {
        return;
    }

    POMDOG_ASSERT(radius > 0);

    POMDOG_ASSERT(segments >= 3);
    const auto centralAngle = Radian<f32>{math::TwoPi<f32> / segments};
    Vector2 prevPoint = position + Vector2{radius, 0};

    for (int i = 0; i < segments; ++i) {
        const auto rad = centralAngle * static_cast<f32>(i + 1);
        const auto cos = std::cos(rad.get());
        const auto sin = std::sin(rad.get());
        const auto nextPoint = position + (radius * Vector2{cos, sin});
        drawLineImpl(nextPoint, prevPoint, color, color);
        prevPoint = nextPoint;
    }
}

void LineBatchImpl::drawLine(const Vector2& start, const Vector2& end, const Color& color)
{
    drawLineImpl(start, end, color, color);
}

void LineBatchImpl::drawLine(const Vector2& start, const Vector2& end, const Color& startColor, const Color& endColor)
{
    drawLineImpl(start, end, startColor, endColor);
}

void LineBatchImpl::drawLine(const Vector3& start, const Vector3& end, const Color& color)
{
    drawLineImpl(start, end, color, color);
}

void LineBatchImpl::drawLine(const Vector3& start, const Vector3& end, const Color& startColor, const Color& endColor)
{
    drawLineImpl(start, end, startColor, endColor);
}

void LineBatchImpl::drawRectangle(const Rect2D& sourceRect, const Color& color)
{
    drawRectangle(sourceRect, color, color, color, color);
}

void LineBatchImpl::drawRectangle(
    const Rect2D& sourceRect,
    const Color& color1,
    const Color& color2,
    const Color& color3,
    const Color& color4)
{
    if (sourceRect.width <= 0 || sourceRect.height <= 0) {
        return;
    }

    std::array<Vector2, 4> rectVertices = {{
        Vector2{static_cast<f32>(sourceRect.getLeft()), static_cast<f32>(sourceRect.y - sourceRect.height)},
        Vector2{static_cast<f32>(sourceRect.getLeft()), static_cast<f32>(sourceRect.y)},
        Vector2{static_cast<f32>(sourceRect.getRight()), static_cast<f32>(sourceRect.y)},
        Vector2{static_cast<f32>(sourceRect.getRight()), static_cast<f32>(sourceRect.y - sourceRect.height)},
    }};

    drawLineImpl(rectVertices[0], rectVertices[1], color1, color2);
    drawLineImpl(rectVertices[1], rectVertices[2], color2, color3);
    drawLineImpl(rectVertices[2], rectVertices[3], color3, color4);
    drawLineImpl(rectVertices[3], rectVertices[0], color4, color1);
}

void LineBatchImpl::drawRectangle(
    const Matrix3x2& matrix,
    const Rect2D& sourceRect,
    const Color& color)
{
    if (sourceRect.width <= 0 || sourceRect.height <= 0) {
        return;
    }

    std::array<Vector2, 4> rectVertices = {{
        Vector2{static_cast<f32>(sourceRect.getLeft()), static_cast<f32>(sourceRect.y - sourceRect.height)},
        Vector2{static_cast<f32>(sourceRect.getLeft()), static_cast<f32>(sourceRect.y)},
        Vector2{static_cast<f32>(sourceRect.getRight()), static_cast<f32>(sourceRect.y)},
        Vector2{static_cast<f32>(sourceRect.getRight()), static_cast<f32>(sourceRect.y - sourceRect.height)},
    }};

    for (auto& vertex : rectVertices) {
        vertex = math::transform(vertex, matrix);
    }

    drawLineImpl(rectVertices[0], rectVertices[1], color, color);
    drawLineImpl(rectVertices[1], rectVertices[2], color, color);
    drawLineImpl(rectVertices[2], rectVertices[3], color, color);
    drawLineImpl(rectVertices[3], rectVertices[0], color, color);
}

void LineBatchImpl::drawSphere(
    const Vector3& position,
    f32 radius,
    const Color& color,
    i32 segments)
{
    POMDOG_ASSERT(segments >= 4);
    POMDOG_ASSERT(radius >= 0);

    if (radius <= 0) {
        return;
    }

    const auto rings = std::max(segments, 4);
    const auto sectors = std::max(segments, 4);

    POMDOG_ASSERT(sectors > 0);
    POMDOG_ASSERT(rings > 0);

    std::vector<Vector3> sphereVertices;
    sphereVertices.reserve(sectors * (rings - 1) + 2);

    const auto R = 1.0f / static_cast<f32>(rings - 1);
    const auto S = 1.0f / static_cast<f32>(sectors - 1);

    // Create sphere vertices
    sphereVertices.push_back(Vector3{0.0f, 1.0f, 0.0f});
    for (int ring = 1; ring < rings; ++ring) {
        const auto latitude = math::Pi<f32> * ring * R;
        const auto y = std::cos(latitude);
        const auto r = std::sin(latitude);
        for (int s = 0; s < sectors; ++s) {
            auto longitude = math::TwoPi<f32> * s * S;
            auto x = r * std::cos(longitude);
            auto z = r * std::sin(longitude);
            sphereVertices.push_back(Vector3{x, y, z});
        }
    }
    sphereVertices.push_back(Vector3{0.0f, -1.0f, 0.0f});

    // Scaling and translation
    for (auto& v : sphereVertices) {
        v = v * radius + position;
    }

    const auto drawIndices = [&](std::size_t a, std::size_t b) {
        POMDOG_ASSERT(a < sphereVertices.size());
        POMDOG_ASSERT(b < sphereVertices.size());
        const auto& start = sphereVertices[a];
        const auto& end = sphereVertices[b];
        drawLineImpl(start, end, color, color);
    };

    for (int s = 1; s < sectors; ++s) {
        drawIndices(0, s);
    }

    int count = 1;
    for (int s = 2; s < sectors; ++s) {
        for (int ring = 1; ring < rings; ++ring) {
            drawIndices(count, count + 1);
            drawIndices(count, count + sectors);
            ++count;
        }
        POMDOG_ASSERT(count + 1 >= sectors);
        drawIndices(count, (count + 1) - sectors);
        drawIndices(count, count + sectors);
        ++count;
    }

    for (int s = 1; s < sectors; ++s) {
        drawIndices(count, count + 1);
        drawIndices(count, sphereVertices.size() - 1);
        ++count;
    }

    POMDOG_ASSERT(count + 1 >= sectors);
    drawIndices(count, (count + 1) - sectors);
    drawIndices(count, sphereVertices.size() - 1);
}

void LineBatchImpl::drawTriangle(
    const Vector2& point1,
    const Vector2& point2,
    const Vector2& point3,
    const Color& color)
{
    drawTriangleImpl(point1, point2, point3, color, color, color);
}

void LineBatchImpl::drawTriangle(
    const Vector2& point1,
    const Vector2& point2,
    const Vector2& point3,
    const Color& color1,
    const Color& color2,
    const Color& color3)
{
    drawTriangleImpl(point1, point2, point3, color1, color2, color3);
}

} // namespace

LinePipeline::~LinePipeline() = default;

LineBatch::~LineBatch() = default;

std::tuple<std::shared_ptr<LinePipeline>, std::unique_ptr<Error>>
createLinePipeline(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) noexcept
{
    auto pipeline = std::make_shared<LinePipelineImpl>();
    if (auto err = pipeline->initialize(fs, graphicsDevice); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(pipeline), nullptr);
}

std::tuple<std::shared_ptr<LineBatch>, std::unique_ptr<Error>>
createLineBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize) noexcept
{
    auto lineBatch = std::make_shared<LineBatchImpl>();
    if (auto err = lineBatch->initialize(graphicsDevice, batchSize); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(lineBatch), nullptr);
}

} // namespace pomdog
