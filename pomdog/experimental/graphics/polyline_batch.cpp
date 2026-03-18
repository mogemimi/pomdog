// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/polyline_batch.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/shader_loader.h"
#include "pomdog/gpu/blend_descriptor.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/depth_stencil_descriptor.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/index_buffer.h"
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
#include "pomdog/math/radian.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"
#include "pomdog/memory/aligned_new.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

#include "pomdog/gpu/rasterizer_descriptor.h"

using pomdog::memory::AlignedNew;

namespace pomdog {
namespace {

struct PolylineBatchVertex final {
    Vector3 Position;
    pomdog::Color Color;
};

struct alignas(16) PolylineVertex final : public AlignedNew<PolylineVertex> {
    // {xyz_} = position.xyz
    // {___w} = unused
    Vector4 Position;

    // {xyz_} = nextPoint.xyz
    // {___w} = extrusion
    Vector4 NextPointExtrusion;

    // {xyz_} = prevPoint.xyz
    // {___w} = lineThickness
    Vector4 PrevPointThickness;

    // {xyzw} = color.rgba
    Vector4 Color;
};

PolylineVertex MakeVertex(
    const Vector3& position,
    const Vector3& nextPoint,
    const Vector3& prevPoint,
    const Vector4& color,
    f32 extrusion,
    f32 thickness)
{
    PolylineVertex vertex;
    vertex.Position = Vector4{position, 1.0};
    vertex.NextPointExtrusion = Vector4{nextPoint, extrusion};
    vertex.PrevPointThickness = Vector4{prevPoint, thickness};
    vertex.Color = color;
    return vertex;
}

#if 0
#define POMDOG_POLYLINE_DEBUG 1
#endif

} // namespace

// MARK: - PolylineBatch::Impl

class PolylineBatch::Impl {
public:
    static constexpr u32 MaxVertexCount = 8192;
    static constexpr u32 MinVertexCount = 256;
    static constexpr u32 MinIndexCount = (MinVertexCount - 2) * 6;
    static constexpr u32 MaxIndexCount = (MaxVertexCount - 2) * 6;

private:
    std::shared_ptr<gpu::CommandList> commandList;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer;
#ifdef POMDOG_POLYLINE_DEBUG
    std::shared_ptr<gpu::VertexBuffer> debugVertexBuffer;
#endif
    std::shared_ptr<gpu::IndexBuffer> indexBuffer;
    std::shared_ptr<gpu::PipelineState> pipelineState;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer;

    std::vector<PolylineVertex> vertices;
    std::vector<u16> indices;
    u32 startIndexLocation_ = 0;

public:
    Impl() = default;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    void begin(
        const std::shared_ptr<gpu::CommandList>& commandListIn,
        const Matrix4x4& transformMatrix);

    void drawPath(std::vector<PolylineBatchVertex>&& path, bool closed, f32 thickness);

    void end();

    void flush();
};

std::unique_ptr<Error>
PolylineBatch::Impl::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    {
        vertices.reserve(MinVertexCount);

        constexpr auto maxVertexCount = MaxVertexCount;
        if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
                maxVertexCount,
                sizeof(PolylineVertex),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
        else {
            vertexBuffer = std::move(buffer);
        }

#ifdef POMDOG_POLYLINE_DEBUG
        debugVertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
            maxVertexCount, sizeof(PolylineVertex), gpu::BufferUsage::Dynamic);
#endif
    }
    {
        indices.reserve(MinIndexCount);

        constexpr auto maxIndexCount = MaxIndexCount;
        if (auto [buffer, err] = graphicsDevice->createIndexBuffer(
                gpu::IndexFormat::UInt16,
                maxIndexCount,
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create index buffer");
        }
        else {
            indexBuffer = std::move(buffer);
        }
    }
    {
        auto inputLayout = gpu::InputLayoutHelper{}
                               .addFloat4()
                               .addFloat4()
                               .addFloat4()
                               .addFloat4();

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

        auto pipelineStateBuilder = PipelineStateBuilder(graphicsDevice);
        pipelineStateBuilder.setRenderTargetViewFormat(presentationParameters.backBufferFormat);
        pipelineStateBuilder.setDepthStencilViewFormat(presentationParameters.depthStencilFormat);
        pipelineStateBuilder.setVertexShader(std::move(vertexShader));
        pipelineStateBuilder.setPixelShader(std::move(pixelShader));
        pipelineStateBuilder.setInputLayout(inputLayout.createInputLayout());
        pipelineStateBuilder.setPrimitiveTopology(gpu::PrimitiveTopology::TriangleList);
        pipelineStateBuilder.setBlendState(gpu::BlendDescriptor::createNonPremultiplied());
        pipelineStateBuilder.setDepthStencilState(gpu::DepthStencilDescriptor::createDefault());
        pipelineStateBuilder.setRasterizerState(gpu::RasterizerDescriptor::createCullNone());
        pipelineStateBuilder.setConstantBufferBindSlot("TransformMatrix", 0);

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

void PolylineBatch::Impl::begin(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const Matrix4x4& transformMatrix)
{
    POMDOG_ASSERT(commandListIn);
    commandList = commandListIn;

    alignas(16) Matrix4x4 transposedMatrix = math::transpose(transformMatrix);
    constantBuffer->setData(0, gpu::makeByteSpan(transposedMatrix));
}

void PolylineBatch::Impl::end()
{
    if (vertices.empty()) {
        return;
    }

    flush();
    commandList.reset();
    vertices.clear();
    indices.clear();
    startIndexLocation_ = 0;
}

void PolylineBatch::Impl::flush()
{
#if 1
    if (vertices.size() >= 2) {
        vertices[vertices.size() - 1].Color.w *= 0.1f;
        vertices[vertices.size() - 2].Color.w *= 0.1f;
    }
#endif

    POMDOG_ASSERT(!vertices.empty());
    POMDOG_ASSERT(vertices.size() <= MaxVertexCount);
    vertexBuffer->setData(vertices.data(), static_cast<u32>(vertices.size()));

    POMDOG_ASSERT(!indices.empty());
    POMDOG_ASSERT(indices.size() <= MaxIndexCount);
    indexBuffer->setData(indices.data(), static_cast<u32>(indices.size()));

#ifdef POMDOG_POLYLINE_DEBUG
    auto vert = vertices;
    for (auto& v : vert) {
        v.Color = Vector4{0.0f, 0.0f, 0.0f, v.Color.W * 0.5f};
    }
    debugVertexBuffer->SetData(vert.data(), vert.size());
#endif

    commandList->setPipelineState(pipelineState);
    commandList->setConstantBuffer(0, constantBuffer);
    commandList->setVertexBuffer(0, vertexBuffer);
    commandList->setIndexBuffer(indexBuffer);
    commandList->drawIndexed(static_cast<u32>(indices.size()), startIndexLocation_);

#ifdef POMDOG_POLYLINE_DEBUG
    commandList->setVertexBuffer(debugVertexBuffer);
    commandList->setPrimitiveTopology(gpu::PrimitiveTopology::LineStrip);
    commandList->drawIndexed(indexBuffer, static_cast<u32>(indices.size()), startIndexLocation);
#endif

    startIndexLocation_ = static_cast<u32>(indices.size());
}

void PolylineBatch::Impl::drawPath(std::vector<PolylineBatchVertex>&& path, bool closed, f32 thickness)
{
    const auto n = static_cast<int>(path.size());

    if ((vertices.size() + path.size() * 2) > MaxVertexCount) {
        flush();
        return;
    }

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
                vertices.push_back(MakeVertex(v.Position, next, prevPoint, color, -1.0f, thickness));
                vertices.push_back(MakeVertex(v.Position, next, prevPoint, color, 1.0f, thickness));

                const auto indexOffset = static_cast<std::uint16_t>(vertices.size());

                const auto prev = v.Position - v2;
                vertices.push_back(MakeVertex(v.Position, nextPoint, prev, color, -1.0f, thickness));
                vertices.push_back(MakeVertex(v.Position, nextPoint, prev, color, 1.0f, thickness));

                if (i < (n - 1)) {
                    indices.push_back(indexOffset + 0);
                    indices.push_back(indexOffset + 2);
                    indices.push_back(indexOffset + 3);
                    indices.push_back(indexOffset + 3);
                    indices.push_back(indexOffset + 1);
                    indices.push_back(indexOffset + 0);
                }
                continue;
            }
        }

        const auto indexOffset = static_cast<std::uint16_t>(vertices.size());
        vertices.push_back(MakeVertex(v.Position, nextPoint, prevPoint, color, -1.0f, thickness));
        vertices.push_back(MakeVertex(v.Position, nextPoint, prevPoint, color, 1.0f, thickness));

        POMDOG_ASSERT(n >= 2);
        if (i < (n - 1)) {
            indices.push_back(indexOffset + 0);
            indices.push_back(indexOffset + 2);
            indices.push_back(indexOffset + 3);
            indices.push_back(indexOffset + 3);
            indices.push_back(indexOffset + 1);
            indices.push_back(indexOffset + 0);
        }
    }

    if (closed) {
        POMDOG_ASSERT(path.size() >= 3);
        const auto indexOffset = static_cast<std::uint16_t>(vertices.size() - 2);
        indices.push_back(indexOffset + 0);
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(1);
        indices.push_back(indexOffset + 1);
        indices.push_back(indexOffset + 0);
    }

    POMDOG_ASSERT(vertices.size() <= MaxVertexCount);
    POMDOG_ASSERT(indices.size() <= MaxIndexCount);
}

// MARK: - PolylineBatch

PolylineBatch::PolylineBatch() = default;

PolylineBatch::~PolylineBatch() = default;

std::unique_ptr<Error>
PolylineBatch::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    impl = std::make_unique<Impl>();
    return impl->initialize(fs, graphicsDevice);
}

void PolylineBatch::begin(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const Matrix4x4& transformMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->begin(commandListIn, transformMatrixIn);
}

void PolylineBatch::end()
{
    POMDOG_ASSERT(impl);
    impl->end();
}

void PolylineBatch::drawPath(const std::vector<Vector2>& path, bool closed, const Color& color, f32 thickness)
{
    POMDOG_ASSERT(impl);
    std::vector<PolylineBatchVertex> vertices;
    for (const auto& v : path) {
        vertices.push_back(PolylineBatchVertex{Vector3{v, 0.0f}, color});
    }
    if (vertices.size() <= 2) {
        closed = false;
    }
    impl->drawPath(std::move(vertices), closed, thickness);
}

void PolylineBatch::drawBox(const BoundingBox& box, const Color& color, f32 thickness)
{
    drawBox(box.min, box.max - box.min, Vector3::createZero(), color, thickness);
}

void PolylineBatch::drawBox(
    const Vector3& position,
    const Vector3& scale,
    const Color& color,
    f32 thickness)
{
    drawBox(position, scale, Vector3::createZero(), color, thickness);
}

void PolylineBatch::drawBox(
    const Vector3& position,
    const Vector3& scale,
    const Vector3& originPivot,
    const Color& color,
    f32 thickness)
{
    POMDOG_ASSERT(impl);

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

void PolylineBatch::drawCircle(const Vector2& position, f32 radius, const Color& color, i32 segments, f32 thickness)
{
    POMDOG_ASSERT(impl);
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

void PolylineBatch::drawLine(const Vector2& start, const Vector2& end, const Color& color, f32 thickness)
{
    POMDOG_ASSERT(impl);
    impl->drawPath(std::vector<PolylineBatchVertex>{
                       PolylineBatchVertex{Vector3{start, 0.0f}, color},
                       PolylineBatchVertex{Vector3{end, 0.0f}, color},
                   },
        false, thickness);
}

void PolylineBatch::drawLine(const Vector2& start, const Vector2& end, const Color& startColor, const Color& endColor, f32 thickness)
{
    POMDOG_ASSERT(impl);
    impl->drawPath(std::vector<PolylineBatchVertex>{
                       PolylineBatchVertex{Vector3{start, 0.0f}, startColor},
                       PolylineBatchVertex{Vector3{end, 0.0f}, endColor},
                   },
        false, thickness);
}

void PolylineBatch::drawLine(const Vector3& start, const Vector3& end, const Color& color, f32 thickness)
{
    POMDOG_ASSERT(impl);
    impl->drawPath(std::vector<PolylineBatchVertex>{
                       PolylineBatchVertex{start, color},
                       PolylineBatchVertex{end, color},
                   },
        false, thickness);
}

void PolylineBatch::drawLine(const Vector3& start, const Vector3& end, const Color& startColor, const Color& endColor, f32 thickness)
{
    POMDOG_ASSERT(impl);
    impl->drawPath(std::vector<PolylineBatchVertex>{
                       PolylineBatchVertex{start, startColor},
                       PolylineBatchVertex{end, endColor},
                   },
        false, thickness);
}

void PolylineBatch::drawRectangle(const Rect2D& sourceRect, const Color& color, f32 thickness)
{
    POMDOG_ASSERT(impl);
    drawRectangle(sourceRect, color, color, color, color, thickness);
}

void PolylineBatch::drawRectangle(
    const Rect2D& sourceRect,
    const Color& color1,
    const Color& color2,
    const Color& color3,
    const Color& color4,
    f32 thickness)
{
    POMDOG_ASSERT(impl);

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

void PolylineBatch::drawRectangle(const Matrix3x2& matrix,
    const Rect2D& sourceRect, const Color& color, f32 thickness)
{
    POMDOG_ASSERT(impl);

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

void PolylineBatch::drawSphere(
    const Vector3& position,
    f32 radius,
    const Color& color,
    i32 segments,
    f32 thickness)
{
    POMDOG_ASSERT(impl);
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

        impl->drawPath(std::move(path1), false, thickness);

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

void PolylineBatch::drawTriangle(const Vector2& point1, const Vector2& point2, const Vector2& point3, const Color& color, f32 thickness)
{
    POMDOG_ASSERT(impl);

    impl->drawPath(std::vector<PolylineBatchVertex>{
                       PolylineBatchVertex{Vector3{point1, 0.0f}, color},
                       PolylineBatchVertex{Vector3{point2, 0.0f}, color},
                       PolylineBatchVertex{Vector3{point2, 0.0f}, color},
                       PolylineBatchVertex{Vector3{point3, 0.0f}, color},
                       PolylineBatchVertex{Vector3{point3, 0.0f}, color},
                       PolylineBatchVertex{Vector3{point1, 0.0f}, color},
                   },
        false, thickness);
}

void PolylineBatch::drawTriangle(
    const Vector2& point1, const Vector2& point2, const Vector2& point3,
    const Color& color1, const Color& color2, const Color& color3,
    f32 thickness)
{
    POMDOG_ASSERT(impl);
    impl->drawPath(std::vector<PolylineBatchVertex>{
                       PolylineBatchVertex{Vector3{point1, 0.0f}, color1},
                       PolylineBatchVertex{Vector3{point2, 0.0f}, color2},
                       PolylineBatchVertex{Vector3{point2, 0.0f}, color2},
                       PolylineBatchVertex{Vector3{point3, 0.0f}, color3},
                       PolylineBatchVertex{Vector3{point3, 0.0f}, color3},
                       PolylineBatchVertex{Vector3{point1, 0.0f}, color1},
                   },
        false, thickness);
}

} // namespace pomdog
