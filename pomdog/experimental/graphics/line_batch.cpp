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
#include "pomdog/memory/unsafe_ptr.h"
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
        0, gpu::InputClassification::PerVertex, 28,
        {
            gpu::InputElementFormat::Float32x3,
            gpu::InputElementFormat::Float32x4,
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
    static constexpr u32 MaxVertexCount = 4096;
    static constexpr u32 MinVertexCount = 256;

    struct Vertex final {
        // {xyz} = position.xyz
        Vector3 Position;

        // {xyzw} = color.rgba
        Vector4 Color;
    };

private:
    std::vector<Vertex> vertices_;
    std::shared_ptr<gpu::CommandList> commandList_;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer_;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer_;
    unsafe_ptr<LinePipelineImpl> currentPipeline_ = nullptr;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    void begin(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<LinePipeline>& linePipeline,
        const Matrix4x4& transformMatrix) override;

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

    void end() override;

private:
    void drawLineImpl(
        const Vector2& point1, const Vector2& point2,
        const Vector4& color1, const Vector4& color2);

    void drawLineImpl(
        const Vector3& point1, const Vector3& point2,
        const Vector4& color1, const Vector4& color2);

    void drawTriangleImpl(
        const Vector2& point1, const Vector2& point2, const Vector2& point3,
        const Vector4& color1, const Vector4& color2, const Vector4& color3);

    void flush();
};

std::unique_ptr<Error>
LineBatchImpl::initialize(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    vertices_.reserve(MinVertexCount);
    {
        auto maxVertexCount = MaxVertexCount;

        if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
                maxVertexCount,
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

void LineBatchImpl::begin(
    const std::shared_ptr<gpu::CommandList>& commandList,
    const std::shared_ptr<LinePipeline>& linePipeline,
    const Matrix4x4& transformMatrix)
{
    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(linePipeline);
    commandList_ = commandList;
    currentPipeline_ = static_cast<LinePipelineImpl*>(linePipeline.get());

    alignas(16) Matrix4x4 transposedMatrix = math::transpose(transformMatrix);
    constantBuffer_->setData(0, gpu::makeByteSpan(transposedMatrix));
}

void LineBatchImpl::end()
{
    if (vertices_.empty()) {
        return;
    }

    flush();
    commandList_.reset();
    currentPipeline_ = nullptr;
}

void LineBatchImpl::flush()
{
    POMDOG_ASSERT(!vertices_.empty());
    POMDOG_ASSERT(vertices_.size() <= MaxVertexCount);
    vertexBuffer_->setData(vertices_.data(), static_cast<u32>(vertices_.size()));

    POMDOG_ASSERT(currentPipeline_);
    commandList_->setVertexBuffer(0, vertexBuffer_);
    commandList_->setPipelineState(currentPipeline_->pipelineState_);
    commandList_->setConstantBuffer(0, constantBuffer_);
    commandList_->draw(static_cast<u32>(vertices_.size()), 0);

    vertices_.clear();
}

void LineBatchImpl::drawLineImpl(
    const Vector2& point1, const Vector2& point2,
    const Vector4& color1, const Vector4& color2)
{
    if (vertices_.size() + 2 > MaxVertexCount) {
        flush();
    }

    POMDOG_ASSERT(vertices_.size() + 2 <= MaxVertexCount);
    vertices_.push_back(Vertex{Vector3(point1, 0.0f), color1});
    vertices_.push_back(Vertex{Vector3(point2, 0.0f), color2});
}

void LineBatchImpl::drawLineImpl(
    const Vector3& point1, const Vector3& point2,
    const Vector4& color1, const Vector4& color2)
{
    if (vertices_.size() + 2 > MaxVertexCount) {
        flush();
    }

    POMDOG_ASSERT(vertices_.size() + 2 <= MaxVertexCount);
    vertices_.push_back(Vertex{point1, color1});
    vertices_.push_back(Vertex{point2, color2});
}

void LineBatchImpl::drawTriangleImpl(
    const Vector2& point1, const Vector2& point2, const Vector2& point3,
    const Vector4& color1, const Vector4& color2, const Vector4& color3)
{
    if (vertices_.size() + 6 > MaxVertexCount) {
        flush();
    }

    POMDOG_ASSERT(vertices_.size() + 6 <= MaxVertexCount);
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

    const auto colorVector = color.toVector4();
    auto draw = [&](int a, int b) {
        drawLineImpl(
            boxVertices[a],
            boxVertices[b],
            colorVector,
            colorVector);
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

    auto colorVector = color.toVector4();

    for (int i = 0; i < segments; ++i) {
        const auto rad = centralAngle * static_cast<f32>(i + 1);
        const auto cos = std::cos(rad.get());
        const auto sin = std::sin(rad.get());
        const auto nextPoint = position + (radius * Vector2{cos, sin});
        drawLineImpl(nextPoint, prevPoint, colorVector, colorVector);
        prevPoint = nextPoint;
    }
}

void LineBatchImpl::drawLine(const Vector2& start, const Vector2& end, const Color& color)
{
    const auto colorVector = color.toVector4();
    drawLineImpl(start, end, colorVector, colorVector);
}

void LineBatchImpl::drawLine(const Vector2& start, const Vector2& end, const Color& startColor, const Color& endColor)
{
    const auto colorVector1 = startColor.toVector4();
    const auto colorVector2 = endColor.toVector4();
    drawLineImpl(start, end, colorVector1, colorVector2);
}

void LineBatchImpl::drawLine(const Vector3& start, const Vector3& end, const Color& color)
{
    const auto colorVector = color.toVector4();
    drawLineImpl(start, end, colorVector, colorVector);
}

void LineBatchImpl::drawLine(const Vector3& start, const Vector3& end, const Color& startColor, const Color& endColor)
{
    const auto colorVector1 = startColor.toVector4();
    const auto colorVector2 = endColor.toVector4();
    drawLineImpl(start, end, colorVector1, colorVector2);
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

    const auto colorVector1 = color1.toVector4();
    const auto colorVector2 = color2.toVector4();
    const auto colorVector3 = color3.toVector4();
    const auto colorVector4 = color4.toVector4();

    drawLineImpl(rectVertices[0], rectVertices[1], colorVector1, colorVector2);
    drawLineImpl(rectVertices[1], rectVertices[2], colorVector2, colorVector3);
    drawLineImpl(rectVertices[2], rectVertices[3], colorVector3, colorVector4);
    drawLineImpl(rectVertices[3], rectVertices[0], colorVector4, colorVector1);
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

    const auto colorVector = color.toVector4();

    drawLineImpl(rectVertices[0], rectVertices[1], colorVector, colorVector);
    drawLineImpl(rectVertices[1], rectVertices[2], colorVector, colorVector);
    drawLineImpl(rectVertices[2], rectVertices[3], colorVector, colorVector);
    drawLineImpl(rectVertices[3], rectVertices[0], colorVector, colorVector);
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

    const auto colorVector = color.toVector4();
    const auto drawIndices = [&](std::size_t a, std::size_t b) {
        POMDOG_ASSERT(a < sphereVertices.size());
        POMDOG_ASSERT(b < sphereVertices.size());
        const auto& start = sphereVertices[a];
        const auto& end = sphereVertices[b];
        drawLineImpl(start, end, colorVector, colorVector);
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
    const auto colorVector = color.toVector4();
    drawTriangleImpl(point1, point2, point3, colorVector, colorVector, colorVector);
}

void LineBatchImpl::drawTriangle(
    const Vector2& point1,
    const Vector2& point2,
    const Vector2& point3,
    const Color& color1,
    const Color& color2,
    const Color& color3)
{
    const auto colorVector1 = color1.toVector4();
    const auto colorVector2 = color2.toVector4();
    const auto colorVector3 = color3.toVector4();
    drawTriangleImpl(point1, point2, point3, colorVector1, colorVector2, colorVector3);
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
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) noexcept
{
    auto lineBatch = std::make_shared<LineBatchImpl>();
    if (auto err = lineBatch->initialize(graphicsDevice); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(lineBatch), nullptr);
}

} // namespace pomdog
