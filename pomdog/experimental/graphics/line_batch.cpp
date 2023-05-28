// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/line_batch.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/asset_builders/shader_builder.h"
#include "pomdog/content/asset_manager.h"
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
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/math/bounding_box.h"
#include "pomdog/math/color.h"
#include "pomdog/math/math.h"
#include "pomdog/math/matrix3x2.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

// Built-in shaders
#include "shaders/glsl.embedded/line_batch_ps.inc.h"
#include "shaders/glsl.embedded/line_batch_vs.inc.h"
#include "shaders/hlsl.embedded/line_batch_ps.inc.h"
#include "shaders/hlsl.embedded/line_batch_vs.inc.h"
#include "shaders/metal.embedded/line_batch.inc.h"

} // namespace

// MARK: - LineBatch::Impl

class LineBatch::Impl {
public:
    static constexpr std::size_t MaxVertexCount = 4096;
    static constexpr std::size_t MinVertexCount = 256;

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
    Impl(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        AssetManager& assets);

    void Begin(
        const std::shared_ptr<gpu::CommandList>& commandListIn,
        const Matrix4x4& transformMatrix);

    void DrawLine(
        const Vector2& point1, const Vector2& point2,
        const Vector4& color1, const Vector4& color2);

    void DrawLine(
        const Vector3& point1, const Vector3& point2,
        const Vector4& color1, const Vector4& color2);

    void DrawTriangle(
        const Vector2& point1, const Vector2& point2, const Vector2& point3,
        const Vector4& color1, const Vector4& color2, const Vector4& color3);

    void End();

    void Flush();
};

LineBatch::Impl::Impl(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
{
    vertices.reserve(MinVertexCount);
    {
        auto maxVertexCount = MaxVertexCount;

        vertexBuffer = std::get<0>(graphicsDevice->CreateVertexBuffer(
            maxVertexCount,
            sizeof(Vertex),
            gpu::BufferUsage::Dynamic));
    }
    {
        auto inputLayout = gpu::InputLayoutHelper{}
                               .Float3()
                               .Float4();

        auto vertexShaderBuilder = assets.CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader)
                                       .SetGLSL(Builtin_GLSL_LineBatch_VS, std::strlen(Builtin_GLSL_LineBatch_VS))
                                       .SetHLSLPrecompiled(BuiltinHLSL_LineBatch_VS, sizeof(BuiltinHLSL_LineBatch_VS))
                                       .SetMetal(Builtin_Metal_LineBatch, std::strlen(Builtin_Metal_LineBatch), "LineBatchVS");

        auto pixelShaderBuilder = assets.CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader)
                                      .SetGLSL(Builtin_GLSL_LineBatch_PS, std::strlen(Builtin_GLSL_LineBatch_PS))
                                      .SetHLSLPrecompiled(BuiltinHLSL_LineBatch_PS, sizeof(BuiltinHLSL_LineBatch_PS))
                                      .SetMetal(Builtin_Metal_LineBatch, std::strlen(Builtin_Metal_LineBatch), "LineBatchPS");

        auto [vertexShader, vertexShaderErr] = vertexShaderBuilder.Build();
        if (vertexShaderErr != nullptr) {
            // FIXME: error handling
        }

        auto [pixelShader, pixelShaderErr] = pixelShaderBuilder.Build();
        if (pixelShaderErr != nullptr) {
            // FIXME: error handling
        }

        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        std::unique_ptr<Error> pipelineStateErr;
        std::tie(pipelineState, pipelineStateErr) = assets.CreateBuilder<gpu::PipelineState>()
                                                        .SetRenderTargetViewFormat(presentationParameters.backBufferFormat)
                                                        .SetDepthStencilViewFormat(presentationParameters.depthStencilFormat)
                                                        .SetVertexShader(std::move(vertexShader))
                                                        .SetPixelShader(std::move(pixelShader))
                                                        .SetInputLayout(inputLayout.CreateInputLayout())
                                                        .SetPrimitiveTopology(gpu::PrimitiveTopology::LineList)
                                                        .SetBlendState(gpu::BlendDescriptor::createNonPremultiplied())
                                                        .SetDepthStencilState(gpu::DepthStencilDescriptor::createDefault())
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

void LineBatch::Impl::Begin(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const Matrix4x4& transformMatrix)
{
    POMDOG_ASSERT(commandListIn);
    commandList = commandListIn;

    alignas(16) Matrix4x4 transposedMatrix = math::transpose(transformMatrix);
    constantBuffer->setData(0, gpu::makeByteSpan(transposedMatrix));
}

void LineBatch::Impl::End()
{
    if (vertices.empty()) {
        return;
    }

    Flush();
    commandList.reset();
}

void LineBatch::Impl::Flush()
{
    POMDOG_ASSERT(!vertices.empty());
    POMDOG_ASSERT(vertices.size() <= MaxVertexCount);
    vertexBuffer->setData(vertices.data(), vertices.size());

    commandList->SetVertexBuffer(0, vertexBuffer);
    commandList->SetPipelineState(pipelineState);
    commandList->SetConstantBuffer(0, constantBuffer);
    commandList->Draw(vertices.size(), 0);

    vertices.clear();
}

void LineBatch::Impl::DrawLine(
    const Vector2& point1, const Vector2& point2,
    const Vector4& color1, const Vector4& color2)
{
    if (vertices.size() + 2 > MaxVertexCount) {
        Flush();
    }

    POMDOG_ASSERT(vertices.size() + 2 <= Impl::MaxVertexCount);
    vertices.push_back(Vertex{Vector3(point1, 0.0f), color1});
    vertices.push_back(Vertex{Vector3(point2, 0.0f), color2});
}

void LineBatch::Impl::DrawLine(
    const Vector3& point1, const Vector3& point2,
    const Vector4& color1, const Vector4& color2)
{
    if (vertices.size() + 2 > MaxVertexCount) {
        Flush();
    }

    POMDOG_ASSERT(vertices.size() + 2 <= Impl::MaxVertexCount);
    vertices.push_back(Vertex{point1, color1});
    vertices.push_back(Vertex{point2, color2});
}

void LineBatch::Impl::DrawTriangle(
    const Vector2& point1, const Vector2& point2, const Vector2& point3,
    const Vector4& color1, const Vector4& color2, const Vector4& color3)
{
    if (vertices.size() + 6 > MaxVertexCount) {
        Flush();
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

LineBatch::LineBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
    : impl(std::make_unique<Impl>(graphicsDevice, assets))
{
}

LineBatch::~LineBatch() = default;

void LineBatch::Begin(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const Matrix4x4& transformMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->Begin(commandListIn, transformMatrixIn);
}

void LineBatch::End()
{
    POMDOG_ASSERT(impl);
    impl->End();
}

void LineBatch::DrawBox(const BoundingBox& box, const Color& color)
{
    this->DrawBox(box.min, box.max - box.min, Vector3::createZero(), color);
}

void LineBatch::DrawBox(
    const Vector3& position,
    const Vector3& scale,
    const Color& color)
{
    this->DrawBox(position, scale, Vector3::createZero(), color);
}

void LineBatch::DrawBox(
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
        impl->DrawLine(
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

void LineBatch::DrawCircle(const Vector2& position, float radius, const Color& color, int segments)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(segments >= 3);
    POMDOG_ASSERT(radius >= 0);

    if (radius <= 0) {
        return;
    }

    POMDOG_ASSERT(radius > 0);

    POMDOG_ASSERT(segments >= 3);
    const auto centralAngle = Radian<float>{math::TwoPi<float> / segments};
    Vector2 prevPoint = position + Vector2{radius, 0};

    auto colorVector = color.toVector4();

    for (int i = 0; i < segments; ++i) {
        const auto rad = centralAngle * static_cast<float>(i + 1);
        const auto cos = std::cos(rad.value);
        const auto sin = std::sin(rad.value);
        const auto nextPoint = position + (radius * Vector2{cos, sin});
        impl->DrawLine(nextPoint, prevPoint, colorVector, colorVector);
        prevPoint = nextPoint;
    }
}

void LineBatch::DrawLine(const Vector2& start, const Vector2& end, const Color& color)
{
    POMDOG_ASSERT(impl);
    const auto colorVector = color.toVector4();
    impl->DrawLine(start, end, colorVector, colorVector);
}

void LineBatch::DrawLine(const Vector2& start, const Vector2& end, const Color& startColor, const Color& endColor)
{
    POMDOG_ASSERT(impl);
    const auto colorVector1 = startColor.toVector4();
    const auto colorVector2 = endColor.toVector4();
    impl->DrawLine(start, end, colorVector1, colorVector2);
}

void LineBatch::DrawLine(const Vector3& start, const Vector3& end, const Color& color)
{
    POMDOG_ASSERT(impl);
    const auto colorVector = color.toVector4();
    impl->DrawLine(start, end, colorVector, colorVector);
}

void LineBatch::DrawLine(const Vector3& start, const Vector3& end, const Color& startColor, const Color& endColor)
{
    POMDOG_ASSERT(impl);
    const auto colorVector1 = startColor.toVector4();
    const auto colorVector2 = endColor.toVector4();
    impl->DrawLine(start, end, colorVector1, colorVector2);
}

void LineBatch::DrawRectangle(const Rectangle& sourceRect, const Color& color)
{
    POMDOG_ASSERT(impl);
    DrawRectangle(sourceRect, color, color, color, color);
}

void LineBatch::DrawRectangle(const Rectangle& sourceRect,
    const Color& color1, const Color& color2, const Color& color3, const Color& color4)
{
    POMDOG_ASSERT(impl);

    if (sourceRect.width <= 0 || sourceRect.height <= 0) {
        return;
    }

    std::array<Vector2, 4> rectVertices = {{
        Vector2{static_cast<float>(sourceRect.getLeft()), static_cast<float>(sourceRect.y - sourceRect.height)},
        Vector2{static_cast<float>(sourceRect.getLeft()), static_cast<float>(sourceRect.y)},
        Vector2{static_cast<float>(sourceRect.getRight()), static_cast<float>(sourceRect.y)},
        Vector2{static_cast<float>(sourceRect.getRight()), static_cast<float>(sourceRect.y - sourceRect.height)},
    }};

    const auto colorVector1 = color1.toVector4();
    const auto colorVector2 = color2.toVector4();
    const auto colorVector3 = color3.toVector4();
    const auto colorVector4 = color4.toVector4();

    impl->DrawLine(rectVertices[0], rectVertices[1], colorVector1, colorVector2);
    impl->DrawLine(rectVertices[1], rectVertices[2], colorVector2, colorVector3);
    impl->DrawLine(rectVertices[2], rectVertices[3], colorVector3, colorVector4);
    impl->DrawLine(rectVertices[3], rectVertices[0], colorVector4, colorVector1);
}

void LineBatch::DrawRectangle(const Matrix3x2& matrix,
    const Rectangle& sourceRect, const Color& color)
{
    POMDOG_ASSERT(impl);

    if (sourceRect.width <= 0 || sourceRect.height <= 0) {
        return;
    }

    std::array<Vector2, 4> rectVertices = {{
        Vector2{static_cast<float>(sourceRect.getLeft()), static_cast<float>(sourceRect.y - sourceRect.height)},
        Vector2{static_cast<float>(sourceRect.getLeft()), static_cast<float>(sourceRect.y)},
        Vector2{static_cast<float>(sourceRect.getRight()), static_cast<float>(sourceRect.y)},
        Vector2{static_cast<float>(sourceRect.getRight()), static_cast<float>(sourceRect.y - sourceRect.height)},
    }};

    for (auto& vertex : rectVertices) {
        vertex = math::transform(vertex, matrix);
    }

    const auto colorVector = color.toVector4();

    impl->DrawLine(rectVertices[0], rectVertices[1], colorVector, colorVector);
    impl->DrawLine(rectVertices[1], rectVertices[2], colorVector, colorVector);
    impl->DrawLine(rectVertices[2], rectVertices[3], colorVector, colorVector);
    impl->DrawLine(rectVertices[3], rectVertices[0], colorVector, colorVector);
}

void LineBatch::DrawSphere(
    const Vector3& position,
    float radius,
    const Color& color,
    int segments)
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

    const auto R = 1.0f / static_cast<float>(rings - 1);
    const auto S = 1.0f / static_cast<float>(sectors - 1);

    // Create sphere vertices
    sphereVertices.push_back(Vector3{0.0f, 1.0f, 0.0f});
    for (int ring = 1; ring < rings; ++ring) {
        const auto latitude = math::Pi<float> * ring * R;
        const auto y = std::cos(latitude);
        const auto r = std::sin(latitude);
        for (int s = 0; s < sectors; ++s) {
            auto longitude = math::TwoPi<float> * s * S;
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
        impl->DrawLine(start, end, colorVector, colorVector);
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

void LineBatch::DrawTriangle(const Vector2& point1, const Vector2& point2, const Vector2& point3, const Color& color)
{
    POMDOG_ASSERT(impl);
    const auto colorVector = color.toVector4();
    impl->DrawTriangle(point1, point2, point3, colorVector, colorVector, colorVector);
}

void LineBatch::DrawTriangle(
    const Vector2& point1, const Vector2& point2, const Vector2& point3,
    const Color& color1, const Color& color2, const Color& color3)
{
    POMDOG_ASSERT(impl);
    const auto colorVector1 = color1.toVector4();
    const auto colorVector2 = color2.toVector4();
    const auto colorVector3 = color3.toVector4();
    impl->DrawTriangle(point1, point2, point3, colorVector1, colorVector2, colorVector3);
}

} // namespace pomdog
