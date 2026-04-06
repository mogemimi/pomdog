// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/polyline_batch.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/shader_loader.h"
#include "pomdog/gpu/blend_desc.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/depth_stencil_desc.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/index_buffer.h"
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

#include "pomdog/gpu/rasterizer_desc.h"

namespace pomdog {
namespace {

struct PolylineBatchVertex final {
    Vector3 Position;
    pomdog::Color Color;
};

struct PolylineVertex final {
    // {xyz_} = position.xyz
    // {___w} = unused
    Vector4 position;

    // {xyz_} = nextPoint.xyz
    // {___w} = extrusion
    Vector4 nextPointExtrusion;

    // {xyz_} = prevPoint.xyz
    // {___w} = lineThickness
    Vector4 prevPointThickness;

    // {xyzw} = color.rgba
    Vector4 color;
};

PolylineVertex MakeVertex(
    const Vector3& position,
    const Vector3& nextPoint,
    const Vector3& prevPoint,
    const Vector4& color,
    f32 extrusion,
    f32 thickness)
{
    PolylineVertex vertex = {};
    vertex.position = Vector4{position, 1.0};
    vertex.nextPointExtrusion = Vector4{nextPoint, extrusion};
    vertex.prevPointThickness = Vector4{prevPoint, thickness};
    vertex.color = color;
    return vertex;
}

#if 0
#define POMDOG_POLYLINE_DEBUG 1
#endif

class PolylinePipelineImpl final : public PolylinePipeline {
public:
    std::shared_ptr<gpu::PipelineState> pipelineState_;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);
};

std::unique_ptr<Error>
PolylinePipelineImpl::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    auto [vertexShader, vsErr] = loadShaderAutomagically(
        fs, graphicsDevice,
        gpu::ShaderPipelineStage::VertexShader,
        "/assets/shaders", "polyline_batch_vs", "polyline_batch_vs");
    if (vsErr != nullptr) {
        return errors::wrap(std::move(vsErr), "failed to load vertex shader");
    }

    auto [pixelShader, psErr] = loadShaderAutomagically(
        fs, graphicsDevice,
        gpu::ShaderPipelineStage::PixelShader,
        "/assets/shaders", "polyline_batch_ps", "polyline_batch_ps");
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
        0, gpu::InputClassification::PerVertex, 64,
        {
            gpu::InputElementFormat::Float32x4,
            gpu::InputElementFormat::Float32x4,
            gpu::InputElementFormat::Float32x4,
            gpu::InputElementFormat::Float32x4,
        });
    pipelineDesc.primitiveTopology = gpu::PrimitiveTopology::TriangleList;
    pipelineDesc.blendState = gpu::BlendDesc::createNonPremultiplied();
    pipelineDesc.depthStencilState = gpu::DepthStencilDesc::createDefault();
    pipelineDesc.rasterizerState = gpu::RasterizerDesc::createCullNone();
    pipelineDesc.multiSampleMask = std::numeric_limits<u32>::max();

    auto [pipeline, pipelineErr] = graphicsDevice->createPipelineState(pipelineDesc);
    if (pipelineErr != nullptr) {
        return errors::wrap(std::move(pipelineErr), "failed to create pipeline state");
    }
    pipelineState_ = std::move(pipeline);

    return nullptr;
}

class PolylineBatchImpl final : public PolylineBatch {
public:
    static constexpr u32 DefaultMaxVertexCount = 8192;
    static constexpr u32 MinVertexCount = 256;
    static constexpr u32 MinIndexCount = (MinVertexCount - 2) * 6;

private:
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer_;
#ifdef POMDOG_POLYLINE_DEBUG
    std::shared_ptr<gpu::VertexBuffer> debugVertexBuffer_;
#endif
    std::shared_ptr<gpu::IndexBuffer> indexBuffer_;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer_;

    std::vector<PolylineVertex> vertices_;
    std::vector<u16> indices_;
    u32 maxVertexCount_ = 0;
    u32 maxIndexCount_ = 0;
    u32 drawCallCount_ = 0;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<u32> batchSize);

    void reset() override;
    void setTransform(const Matrix4x4& transformMatrix) override;

    void drawPath(const std::vector<Vector2>& path, bool closed, const Color& color, f32 thickness, std::optional<Color> endColor) override;

    void drawBox(const BoundingBox& box, const Color& color, f32 thickness) override;
    void drawBox(const Vector3& position, const Vector3& scale, const Color& color, f32 thickness) override;
    void drawBox(const Vector3& position, const Vector3& scale, const Vector3& originPivot, const Color& color, f32 thickness) override;

    void drawCircle(const Vector2& position, f32 radius, const Color& color, i32 segments, f32 thickness) override;

    void drawLine(const Vector2& start, const Vector2& end, const Color& color, f32 thickness) override;
    void drawLine(const Vector2& start, const Vector2& end, const Color& startColor, const Color& endColor, f32 thickness) override;
    void drawLine(const Vector3& start, const Vector3& end, const Color& color, f32 thickness) override;
    void drawLine(const Vector3& start, const Vector3& end, const Color& startColor, const Color& endColor, f32 thickness) override;

    void drawRectangle(const Rect2D& sourceRect, const Color& color, f32 thickness) override;
    void drawRectangle(const Rect2D& sourceRect, const Color& color1, const Color& color2, const Color& color3, const Color& color4, f32 thickness) override;
    void drawRectangle(const Matrix3x2& matrix, const Rect2D& sourceRect, const Color& color, f32 thickness) override;

    void drawSphere(const Vector3& position, f32 radius, const Color& color, i32 segments, f32 thickness) override;

    void drawTriangle(const Vector2& point1, const Vector2& point2, const Vector2& point3, const Color& color, f32 thickness) override;
    void drawTriangle(const Vector2& point1, const Vector2& point2, const Vector2& point3, const Color& color1, const Color& color2, const Color& color3, f32 thickness) override;

    void flush(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<PolylinePipeline>& polylinePipeline) override;
    void submit(const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) override;
    [[nodiscard]] u32 getDrawCallCount() const noexcept override;

private:
    void drawPathImpl(std::vector<PolylineBatchVertex>&& path, bool closed, f32 thickness);
};

std::unique_ptr<Error>
PolylineBatchImpl::initialize(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize)
{
    maxVertexCount_ = batchSize.value_or(DefaultMaxVertexCount);
    maxIndexCount_ = (maxVertexCount_ - 2) * 6;

    if (maxVertexCount_ < MinVertexCount) {
        return errors::make("batch size must be at least " + std::to_string(MinVertexCount));
    }
    static_assert(MinVertexCount >= 3, "batch size must be at least 3 to draw a line");
    static_assert((MinVertexCount - 2) * 6 >= MinIndexCount, "index buffer size must be sufficient for the vertex buffer");
    POMDOG_ASSERT(maxIndexCount_ >= MinIndexCount);

    {
        vertices_.reserve(MinVertexCount);

        if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
                maxVertexCount_,
                sizeof(PolylineVertex),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
        else {
            vertexBuffer_ = std::move(buffer);
        }

#ifdef POMDOG_POLYLINE_DEBUG
        debugVertexBuffer_ = std::make_shared<VertexBuffer>(graphicsDevice,
            maxVertexCount_, sizeof(PolylineVertex), gpu::BufferUsage::Dynamic);
#endif
    }
    {
        indices_.reserve(MinIndexCount);

        if (auto [buffer, err] = graphicsDevice->createIndexBuffer(
                gpu::IndexFormat::UInt16,
                maxIndexCount_,
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create index buffer");
        }
        else {
            indexBuffer_ = std::move(buffer);
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

void PolylineBatchImpl::reset()
{
    vertices_.clear();
    indices_.clear();
    drawCallCount_ = 0;
}

void PolylineBatchImpl::setTransform(const Matrix4x4& transformMatrix)
{
    alignas(16) Matrix4x4 transposedMatrix = math::transpose(transformMatrix);
    constantBuffer_->setData(0, gpu::makeByteSpan(transposedMatrix));
}

void PolylineBatchImpl::flush(
    const std::shared_ptr<gpu::CommandList>& commandList,
    const std::shared_ptr<PolylinePipeline>& polylinePipeline)
{
    if (vertices_.empty() || indices_.empty()) {
        return;
    }

    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(polylinePipeline);

    auto* pipeline = static_cast<PolylinePipelineImpl*>(polylinePipeline.get());

    commandList->setPipelineState(pipeline->pipelineState_);
    commandList->setConstantBuffer(0, constantBuffer_);
    commandList->setVertexBuffer(0, vertexBuffer_);
    commandList->setIndexBuffer(indexBuffer_);
    commandList->drawIndexed(static_cast<u32>(indices_.size()), 0);

#ifdef POMDOG_POLYLINE_DEBUG
    commandList->setVertexBuffer(debugVertexBuffer_);
    commandList->setPrimitiveTopology(gpu::PrimitiveTopology::LineStrip);
    commandList->drawIndexed(indexBuffer_, static_cast<u32>(indices_.size()), 0);
#endif

    ++drawCallCount_;
}

void PolylineBatchImpl::submit(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    if (vertices_.empty()) {
        return;
    }

    const auto vertexCount = static_cast<u32>(vertices_.size());
    const auto indexCount = static_cast<u32>(indices_.size());

    if (vertexCount > maxVertexCount_) {
        auto requiredVertexCount = vertexCount + 2048;
        if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
                requiredVertexCount,
                sizeof(PolylineVertex),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return;
        }
        else {
            vertexBuffer_ = std::move(buffer);
            maxVertexCount_ = requiredVertexCount;
        }
    }

    if (indexCount > maxIndexCount_) {
        auto requiredIndexCount = indexCount + 2048;
        if (auto [buffer, err] = graphicsDevice->createIndexBuffer(
                gpu::IndexFormat::UInt16,
                requiredIndexCount,
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return;
        }
        else {
            indexBuffer_ = std::move(buffer);
            maxIndexCount_ = requiredIndexCount;
        }
    }

    vertexBuffer_->setData(vertices_.data(), vertexCount);
    indexBuffer_->setData(indices_.data(), indexCount);

#ifdef POMDOG_POLYLINE_DEBUG
    auto vert = vertices_;
    for (auto& v : vert) {
        v.Color = Vector4{0.0f, 0.0f, 0.0f, v.Color.W * 0.5f};
    }
    debugVertexBuffer_->SetData(vert.data(), vert.size());
#endif
}

u32 PolylineBatchImpl::getDrawCallCount() const noexcept
{
    return drawCallCount_;
}

void PolylineBatchImpl::drawPathImpl(std::vector<PolylineBatchVertex>&& path, bool closed, f32 thickness)
{
    const auto n = static_cast<int>(path.size());

    if (n < 2) {
        return;
    }

    for (int i = 0; i < n; i++) {
        POMDOG_ASSERT(n >= 2);
        const auto& v = path[i];
        auto color = v.Color.toVector4();

        Vector3 prevPoint;
        Vector3 nextPoint;

        if (i == (n - 1)) {
            POMDOG_ASSERT(i >= 1);
            POMDOG_ASSERT(i < static_cast<int>(path.size()));
            if (closed) {
                POMDOG_ASSERT(path.size() >= 3);
                const auto& start = path[i - 1].Position;
                const auto& end = path.front().Position;
                prevPoint = start;
                nextPoint = end;

                if (math::distanceSquared(v.Position, nextPoint) < 0.0001f) {
                    auto dir = v.Position - start;
                    nextPoint = v.Position + dir;
                }
            }
            else {
                const auto& start = path[i - 1].Position;
                const auto& end = path[i].Position;
                auto dir = end - start;
                prevPoint = start;
                nextPoint = end + dir;
            }
        }
        else if (i == 0) {
            POMDOG_ASSERT(i >= 0);
            POMDOG_ASSERT((i + 1) < static_cast<int>(path.size()));
            if (closed) {
                POMDOG_ASSERT(path.size() >= 3);
                const auto& start = path.back().Position;
                const auto& end = path[i + 1].Position;
                prevPoint = start;
                nextPoint = end;

                if (math::distanceSquared(v.Position, nextPoint) < 0.0001f) {
                    auto dir = v.Position - start;
                    nextPoint = v.Position + dir;
                }
            }
            else {
                const auto& start = path[i].Position;
                const auto& end = path[i + 1].Position;
                auto dir = end - start;
                prevPoint = start - dir;
                nextPoint = end;
            }
        }
        else {
            POMDOG_ASSERT(i >= 1);
            POMDOG_ASSERT((i + 1) < static_cast<int>(path.size()));
            const auto& start = path[i - 1].Position;
            const auto& end = path[i + 1].Position;
            prevPoint = start;
            nextPoint = end;

            if (math::distanceSquared(v.Position, nextPoint) < 0.0001f) {
                auto dir = v.Position - start;
                nextPoint = v.Position + dir;
            }
        }

        if (i < (n - 1) || closed) {
            // NOTE: shape corner
            constexpr f32 cutoff = -0.90f;
            const auto v1 = math::normalize(v.Position - prevPoint);
            const auto v2 = math::normalize(nextPoint - v.Position);
            if (math::dot(v2, v1) < cutoff) {
                const auto next = v.Position + v1;
                vertices_.push_back(MakeVertex(v.Position, next, prevPoint, color, -1.0f, thickness));
                vertices_.push_back(MakeVertex(v.Position, next, prevPoint, color, 1.0f, thickness));

                const auto indexOffset = static_cast<std::uint16_t>(vertices_.size());

                const auto prev = v.Position - v2;
                vertices_.push_back(MakeVertex(v.Position, nextPoint, prev, color, -1.0f, thickness));
                vertices_.push_back(MakeVertex(v.Position, nextPoint, prev, color, 1.0f, thickness));

                if (i < (n - 1)) {
                    indices_.push_back(indexOffset + 0);
                    indices_.push_back(indexOffset + 2);
                    indices_.push_back(indexOffset + 3);
                    indices_.push_back(indexOffset + 3);
                    indices_.push_back(indexOffset + 1);
                    indices_.push_back(indexOffset + 0);
                }
                continue;
            }
        }

        const auto indexOffset = static_cast<std::uint16_t>(vertices_.size());
        vertices_.push_back(MakeVertex(v.Position, nextPoint, prevPoint, color, -1.0f, thickness));
        vertices_.push_back(MakeVertex(v.Position, nextPoint, prevPoint, color, 1.0f, thickness));

        POMDOG_ASSERT(n >= 2);
        if (i < (n - 1)) {
            indices_.push_back(indexOffset + 0);
            indices_.push_back(indexOffset + 2);
            indices_.push_back(indexOffset + 3);
            indices_.push_back(indexOffset + 3);
            indices_.push_back(indexOffset + 1);
            indices_.push_back(indexOffset + 0);
        }
    }

    if (closed) {
        POMDOG_ASSERT(path.size() >= 3);
        const auto indexOffset = static_cast<std::uint16_t>(vertices_.size() - 2);
        indices_.push_back(indexOffset + 0);
        indices_.push_back(0);
        indices_.push_back(1);
        indices_.push_back(1);
        indices_.push_back(indexOffset + 1);
        indices_.push_back(indexOffset + 0);
    }

    POMDOG_ASSERT(maxVertexCount_ > 0);
    POMDOG_ASSERT(maxIndexCount_ > 0);
    POMDOG_ASSERT(vertices_.size() <= maxVertexCount_);
    POMDOG_ASSERT(indices_.size() <= maxIndexCount_);
}

void PolylineBatchImpl::drawPath(
    const std::vector<Vector2>& path,
    bool closed,
    const Color& color,
    f32 thickness,
    std::optional<Color> endColor)
{
    std::vector<PolylineBatchVertex> vertices;
    for (const auto& v : path) {
        vertices.push_back(PolylineBatchVertex{Vector3{v, 0.0f}, color});
    }
    if (vertices.size() <= 2) {
        closed = false;
    }
    drawPathImpl(std::move(vertices), closed, thickness);

    if (endColor.has_value()) {
        if (vertices_.size() >= 2) {
            const auto colorVec = endColor->toVector4();
            vertices_[vertices_.size() - 1].color = colorVec;
            vertices_[vertices_.size() - 2].color = colorVec;
        }
    }
}

void PolylineBatchImpl::drawBox(const BoundingBox& box, const Color& color, f32 thickness)
{
    drawBox(box.min, box.max - box.min, Vector3::createZero(), color, thickness);
}

void PolylineBatchImpl::drawBox(
    const Vector3& position,
    const Vector3& scale,
    const Color& color,
    f32 thickness)
{
    drawBox(position, scale, Vector3::createZero(), color, thickness);
}

void PolylineBatchImpl::drawBox(
    const Vector3& position,
    const Vector3& scale,
    const Vector3& originPivot,
    const Color& color,
    f32 thickness)
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
        drawLine(boxVertices[a], boxVertices[b], color, color, thickness);
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

void PolylineBatchImpl::drawCircle(const Vector2& position, f32 radius, const Color& color, i32 segments, f32 thickness)
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
        auto rad = centralAngle * static_cast<f32>(i + 1);
        auto cos = std::cos(rad.get());
        auto sin = std::sin(rad.get());
        auto nextPoint = position + (radius * Vector2{cos, sin});
        drawLine(nextPoint, prevPoint, color, color, thickness);
        prevPoint = nextPoint;
    }
}

void PolylineBatchImpl::drawLine(const Vector2& start, const Vector2& end, const Color& color, f32 thickness)
{
    drawPathImpl(std::vector<PolylineBatchVertex>{
                     PolylineBatchVertex{Vector3{start, 0.0f}, color},
                     PolylineBatchVertex{Vector3{end, 0.0f}, color},
                 },
        false, thickness);
}

void PolylineBatchImpl::drawLine(const Vector2& start, const Vector2& end, const Color& startColor, const Color& endColor, f32 thickness)
{
    drawPathImpl(std::vector<PolylineBatchVertex>{
                     PolylineBatchVertex{Vector3{start, 0.0f}, startColor},
                     PolylineBatchVertex{Vector3{end, 0.0f}, endColor},
                 },
        false, thickness);
}

void PolylineBatchImpl::drawLine(const Vector3& start, const Vector3& end, const Color& color, f32 thickness)
{
    drawPathImpl(std::vector<PolylineBatchVertex>{
                     PolylineBatchVertex{start, color},
                     PolylineBatchVertex{end, color},
                 },
        false, thickness);
}

void PolylineBatchImpl::drawLine(const Vector3& start, const Vector3& end, const Color& startColor, const Color& endColor, f32 thickness)
{
    drawPathImpl(std::vector<PolylineBatchVertex>{
                     PolylineBatchVertex{start, startColor},
                     PolylineBatchVertex{end, endColor},
                 },
        false, thickness);
}

void PolylineBatchImpl::drawRectangle(const Rect2D& sourceRect, const Color& color, f32 thickness)
{
    drawRectangle(sourceRect, color, color, color, color, thickness);
}

void PolylineBatchImpl::drawRectangle(
    const Rect2D& sourceRect,
    const Color& color1,
    const Color& color2,
    const Color& color3,
    const Color& color4,
    f32 thickness)
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

    drawLine(rectVertices[0], rectVertices[1], color1, color2, thickness);
    drawLine(rectVertices[1], rectVertices[2], color2, color3, thickness);
    drawLine(rectVertices[2], rectVertices[3], color3, color4, thickness);
    drawLine(rectVertices[3], rectVertices[0], color4, color1, thickness);
}

void PolylineBatchImpl::drawRectangle(const Matrix3x2& matrix,
    const Rect2D& sourceRect, const Color& color, f32 thickness)
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

    drawLine(rectVertices[0], rectVertices[1], color, color, thickness);
    drawLine(rectVertices[1], rectVertices[2], color, color, thickness);
    drawLine(rectVertices[2], rectVertices[3], color, color, thickness);
    drawLine(rectVertices[3], rectVertices[0], color, color, thickness);
}

void PolylineBatchImpl::drawSphere(
    const Vector3& position,
    f32 radius,
    const Color& color,
    i32 segments,
    f32 thickness)
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
        drawLine(start, end, color, color, thickness);
    };

    for (int s = 1; s < sectors; ++s) {
        drawIndices(0, s);
    }

    int count = 1;
    for (int s = 2; s < sectors; ++s) {
        std::vector<PolylineBatchVertex> path1;

        for (int ring = 1; ring < rings; ++ring) {
            POMDOG_ASSERT(count < static_cast<int>(sphereVertices.size()));
            path1.push_back(PolylineBatchVertex{sphereVertices[count], color});

            drawIndices(count, count + sectors);
            ++count;
        }

        POMDOG_ASSERT(count < static_cast<int>(sphereVertices.size()));
        path1.push_back(PolylineBatchVertex{sphereVertices[count], color});

        drawPathImpl(std::move(path1), false, thickness);

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

void PolylineBatchImpl::drawTriangle(const Vector2& point1, const Vector2& point2, const Vector2& point3, const Color& color, f32 thickness)
{
    drawPathImpl(std::vector<PolylineBatchVertex>{
                     PolylineBatchVertex{Vector3{point1, 0.0f}, color},
                     PolylineBatchVertex{Vector3{point2, 0.0f}, color},
                     PolylineBatchVertex{Vector3{point2, 0.0f}, color},
                     PolylineBatchVertex{Vector3{point3, 0.0f}, color},
                     PolylineBatchVertex{Vector3{point3, 0.0f}, color},
                     PolylineBatchVertex{Vector3{point1, 0.0f}, color},
                 },
        false, thickness);
}

void PolylineBatchImpl::drawTriangle(
    const Vector2& point1, const Vector2& point2, const Vector2& point3,
    const Color& color1, const Color& color2, const Color& color3,
    f32 thickness)
{
    drawPathImpl(std::vector<PolylineBatchVertex>{
                     PolylineBatchVertex{Vector3{point1, 0.0f}, color1},
                     PolylineBatchVertex{Vector3{point2, 0.0f}, color2},
                     PolylineBatchVertex{Vector3{point2, 0.0f}, color2},
                     PolylineBatchVertex{Vector3{point3, 0.0f}, color3},
                     PolylineBatchVertex{Vector3{point3, 0.0f}, color3},
                     PolylineBatchVertex{Vector3{point1, 0.0f}, color1},
                 },
        false, thickness);
}

} // namespace

PolylinePipeline::~PolylinePipeline() = default;

PolylineBatch::~PolylineBatch() = default;

std::tuple<std::shared_ptr<PolylinePipeline>, std::unique_ptr<Error>>
createPolylinePipeline(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) noexcept
{
    auto pipeline = std::make_shared<PolylinePipelineImpl>();
    if (auto err = pipeline->initialize(fs, graphicsDevice); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(pipeline), nullptr);
}

std::tuple<std::shared_ptr<PolylineBatch>, std::unique_ptr<Error>>
createPolylineBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize) noexcept
{
    auto polylineBatch = std::make_shared<PolylineBatchImpl>();
    if (auto err = polylineBatch->initialize(graphicsDevice, batchSize); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(polylineBatch), nullptr);
}

} // namespace pomdog
