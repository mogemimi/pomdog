// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/line_batch.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/shader_loader.h"
#include "pomdog/gpu/blend_desc.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/depth_stencil_desc.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/input_layout_helper.h"
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
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

// MARK: - LineBatch::Impl

class LineBatch::Impl {
public:
    static constexpr u32 MaxVertexCount = 4096;
    static constexpr u32 MinVertexCount = 256;

    struct Vertex {
        // {xyz} = position.xyz
        Vector3 Position;

        // {xyzw} = color.rgba
        Vector4 Color;
    };

    std::vector<Vertex> vertices;

private:
    std::shared_ptr<gpu::CommandList> commandList;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer;
    std::shared_ptr<gpu::PipelineState> pipelineState;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer;

public:
    Impl() = default;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    void begin(
        const std::shared_ptr<gpu::CommandList>& commandListIn,
        const Matrix4x4& transformMatrix);

    void drawLine(
        const Vector2& point1, const Vector2& point2,
        const Vector4& color1, const Vector4& color2);

    void drawLine(
        const Vector3& point1, const Vector3& point2,
        const Vector4& color1, const Vector4& color2);

    void drawTriangle(
        const Vector2& point1, const Vector2& point2, const Vector2& point3,
        const Vector4& color1, const Vector4& color2, const Vector4& color3);

    void end();

    void flush();
};

std::unique_ptr<Error>
LineBatch::Impl::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    vertices.reserve(MinVertexCount);
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

        auto pipelineStateBuilder = PipelineStateBuilder(graphicsDevice);
        pipelineStateBuilder.setRenderTargetViewFormat(presentationParameters.backBufferFormat);
        pipelineStateBuilder.setDepthStencilViewFormat(presentationParameters.depthStencilFormat);
        pipelineStateBuilder.setVertexShader(std::move(vertexShader));
        pipelineStateBuilder.setPixelShader(std::move(pixelShader));
        pipelineStateBuilder.setInputLayout(inputLayout.createInputLayout());
        pipelineStateBuilder.setPrimitiveTopology(gpu::PrimitiveTopology::LineList);
        pipelineStateBuilder.setBlendState(gpu::BlendDesc::createNonPremultiplied());
        pipelineStateBuilder.setDepthStencilState(gpu::DepthStencilDesc::createDefault());

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

void LineBatch::Impl::begin(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const Matrix4x4& transformMatrix)
{
    POMDOG_ASSERT(commandListIn);
    commandList = commandListIn;

    alignas(16) Matrix4x4 transposedMatrix = math::transpose(transformMatrix);
    constantBuffer->setData(0, gpu::makeByteSpan(transposedMatrix));
}

void LineBatch::Impl::end()
{
    if (vertices.empty()) {
        return;
    }

    flush();
    commandList.reset();
}

void LineBatch::Impl::flush()
{
    POMDOG_ASSERT(!vertices.empty());
    POMDOG_ASSERT(vertices.size() <= MaxVertexCount);
    vertexBuffer->setData(vertices.data(), static_cast<u32>(vertices.size()));

    commandList->setVertexBuffer(0, vertexBuffer);
    commandList->setPipelineState(pipelineState);
    commandList->setConstantBuffer(0, constantBuffer);
    commandList->draw(static_cast<u32>(vertices.size()), 0);

    vertices.clear();
}

void LineBatch::Impl::drawLine(
    const Vector2& point1, const Vector2& point2,
    const Vector4& color1, const Vector4& color2)
{
    if (vertices.size() + 2 > MaxVertexCount) {
        flush();
    }

    POMDOG_ASSERT(vertices.size() + 2 <= Impl::MaxVertexCount);
    vertices.push_back(Vertex{Vector3(point1, 0.0f), color1});
    vertices.push_back(Vertex{Vector3(point2, 0.0f), color2});
}

void LineBatch::Impl::drawLine(
    const Vector3& point1, const Vector3& point2,
    const Vector4& color1, const Vector4& color2)
{
    if (vertices.size() + 2 > MaxVertexCount) {
        flush();
    }

    POMDOG_ASSERT(vertices.size() + 2 <= Impl::MaxVertexCount);
    vertices.push_back(Vertex{point1, color1});
    vertices.push_back(Vertex{point2, color2});
}

void LineBatch::Impl::drawTriangle(
    const Vector2& point1, const Vector2& point2, const Vector2& point3,
    const Vector4& color1, const Vector4& color2, const Vector4& color3)
{
    if (vertices.size() + 6 > MaxVertexCount) {
        flush();
    }

    POMDOG_ASSERT(vertices.size() + 6 <= Impl::MaxVertexCount);
    vertices.push_back(Vertex{Vector3(point1, 0.0f), color1});
    vertices.push_back(Vertex{Vector3(point2, 0.0f), color2});
    vertices.push_back(Vertex{Vector3(point2, 0.0f), color2});
    vertices.push_back(Vertex{Vector3(point3, 0.0f), color3});
    vertices.push_back(Vertex{Vector3(point3, 0.0f), color3});
    vertices.push_back(Vertex{Vector3(point1, 0.0f), color1});
}

// MARK: - LineBatch

LineBatch::LineBatch() = default;

LineBatch::~LineBatch() = default;

std::unique_ptr<Error>
LineBatch::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    impl = std::make_unique<Impl>();
    return impl->initialize(fs, graphicsDevice);
}

void LineBatch::begin(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const Matrix4x4& transformMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->begin(commandListIn, transformMatrixIn);
}

void LineBatch::end()
{
    POMDOG_ASSERT(impl);
    impl->end();
}

void LineBatch::drawBox(const BoundingBox& box, const Color& color)
{
    drawBox(box.min, box.max - box.min, Vector3::createZero(), color);
}

void LineBatch::drawBox(
    const Vector3& position,
    const Vector3& scale,
    const Color& color)
{
    drawBox(position, scale, Vector3::createZero(), color);
}

void LineBatch::drawBox(
    const Vector3& position,
    const Vector3& scale,
    const Vector3& originPivot,
    const Color& color)
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

    const auto colorVector = color.toVector4();
    auto draw = [&](int a, int b) {
        impl->drawLine(
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

void LineBatch::drawCircle(const Vector2& position, f32 radius, const Color& color, i32 segments)
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

    auto colorVector = color.toVector4();

    for (int i = 0; i < segments; ++i) {
        const auto rad = centralAngle * static_cast<f32>(i + 1);
        const auto cos = std::cos(rad.get());
        const auto sin = std::sin(rad.get());
        const auto nextPoint = position + (radius * Vector2{cos, sin});
        impl->drawLine(nextPoint, prevPoint, colorVector, colorVector);
        prevPoint = nextPoint;
    }
}

void LineBatch::drawLine(const Vector2& start, const Vector2& end, const Color& color)
{
    POMDOG_ASSERT(impl);
    const auto colorVector = color.toVector4();
    impl->drawLine(start, end, colorVector, colorVector);
}

void LineBatch::drawLine(const Vector2& start, const Vector2& end, const Color& startColor, const Color& endColor)
{
    POMDOG_ASSERT(impl);
    const auto colorVector1 = startColor.toVector4();
    const auto colorVector2 = endColor.toVector4();
    impl->drawLine(start, end, colorVector1, colorVector2);
}

void LineBatch::drawLine(const Vector3& start, const Vector3& end, const Color& color)
{
    POMDOG_ASSERT(impl);
    const auto colorVector = color.toVector4();
    impl->drawLine(start, end, colorVector, colorVector);
}

void LineBatch::drawLine(const Vector3& start, const Vector3& end, const Color& startColor, const Color& endColor)
{
    POMDOG_ASSERT(impl);
    const auto colorVector1 = startColor.toVector4();
    const auto colorVector2 = endColor.toVector4();
    impl->drawLine(start, end, colorVector1, colorVector2);
}

void LineBatch::drawRectangle(const Rect2D& sourceRect, const Color& color)
{
    POMDOG_ASSERT(impl);
    drawRectangle(sourceRect, color, color, color, color);
}

void LineBatch::drawRectangle(
    const Rect2D& sourceRect,
    const Color& color1,
    const Color& color2,
    const Color& color3,
    const Color& color4)
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

    const auto colorVector1 = color1.toVector4();
    const auto colorVector2 = color2.toVector4();
    const auto colorVector3 = color3.toVector4();
    const auto colorVector4 = color4.toVector4();

    impl->drawLine(rectVertices[0], rectVertices[1], colorVector1, colorVector2);
    impl->drawLine(rectVertices[1], rectVertices[2], colorVector2, colorVector3);
    impl->drawLine(rectVertices[2], rectVertices[3], colorVector3, colorVector4);
    impl->drawLine(rectVertices[3], rectVertices[0], colorVector4, colorVector1);
}

void LineBatch::drawRectangle(
    const Matrix3x2& matrix,
    const Rect2D& sourceRect,
    const Color& color)
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

    const auto colorVector = color.toVector4();

    impl->drawLine(rectVertices[0], rectVertices[1], colorVector, colorVector);
    impl->drawLine(rectVertices[1], rectVertices[2], colorVector, colorVector);
    impl->drawLine(rectVertices[2], rectVertices[3], colorVector, colorVector);
    impl->drawLine(rectVertices[3], rectVertices[0], colorVector, colorVector);
}

void LineBatch::drawSphere(
    const Vector3& position,
    f32 radius,
    const Color& color,
    i32 segments)
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

    const auto colorVector = color.toVector4();
    const auto drawIndices = [&](std::size_t a, std::size_t b) {
        POMDOG_ASSERT(a < sphereVertices.size());
        POMDOG_ASSERT(b < sphereVertices.size());
        const auto& start = sphereVertices[a];
        const auto& end = sphereVertices[b];
        impl->drawLine(start, end, colorVector, colorVector);
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

void LineBatch::drawTriangle(
    const Vector2& point1,
    const Vector2& point2,
    const Vector2& point3,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    const auto colorVector = color.toVector4();
    impl->drawTriangle(point1, point2, point3, colorVector, colorVector, colorVector);
}

void LineBatch::drawTriangle(
    const Vector2& point1,
    const Vector2& point2,
    const Vector2& point3,
    const Color& color1,
    const Color& color2,
    const Color& color3)
{
    POMDOG_ASSERT(impl);
    const auto colorVector1 = color1.toVector4();
    const auto colorVector2 = color2.toVector4();
    const auto colorVector3 = color3.toVector4();
    impl->drawTriangle(point1, point2, point3, colorVector1, colorVector2, colorVector3);
}

} // namespace pomdog
