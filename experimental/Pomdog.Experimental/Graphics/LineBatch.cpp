// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "LineBatch.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Math/BoundingBox.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include <algorithm>
#include <cmath>

namespace Pomdog {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/LineBatch_VS.inc.hpp"
#include "Shaders/GLSL.Embedded/LineBatch_PS.inc.hpp"
#include "Shaders/HLSL.Embedded/LineBatch_VS.inc.hpp"
#include "Shaders/HLSL.Embedded/LineBatch_PS.inc.hpp"

} // unnamed namespace
//-----------------------------------------------------------------------
// MARK: - LineBatch::Impl
//-----------------------------------------------------------------------
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
    std::shared_ptr<GraphicsCommandList> commandList;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<ConstantBuffer> constantBuffer;

public:
    Impl(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets);

    void Begin(
        std::shared_ptr<GraphicsCommandList> const& commandListIn,
        Matrix4x4 const& transformMatrix);

    void DrawLine(
        Vector2 const& point1, Vector2 const& point2,
        Vector4 const& color1, Vector4 const& color2);

    void DrawLine(
        Vector3 const& point1, Vector3 const& point2,
        Vector4 const& color1, Vector4 const& color2);

    void DrawTriangle(
        Vector2 const& point1, Vector2 const& point2, Vector2 const& point3,
        Vector4 const& color1, Vector4 const& color2, Vector4 const& color3);

    void End();

    void Flush();
};
//-----------------------------------------------------------------------
LineBatch::Impl::Impl(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
{
    vertices.reserve(MinVertexCount);
    {
        auto maxVertexCount = MaxVertexCount;
        vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
            maxVertexCount, sizeof(Vertex), BufferUsage::Dynamic);
    }
    {
        auto inputLayout = InputLayoutHelper{}
            .Float3().Float4();

        auto vertexShader = assets.CreateBuilder<Shader>(ShaderPipelineStage::VertexShader)
            .SetGLSL(Builtin_GLSL_LineBatch_VS, std::strlen(Builtin_GLSL_LineBatch_VS))
            .SetHLSLPrecompiled(BuiltinHLSL_LineBatch_VS, sizeof(BuiltinHLSL_LineBatch_VS));

        auto pixelShader = assets.CreateBuilder<Shader>(ShaderPipelineStage::PixelShader)
            .SetGLSL(Builtin_GLSL_LineBatch_PS, std::strlen(Builtin_GLSL_LineBatch_PS))
            .SetHLSLPrecompiled(BuiltinHLSL_LineBatch_PS, sizeof(BuiltinHLSL_LineBatch_PS));

        pipelineState = assets.CreateBuilder<PipelineState>()
            .SetVertexShader(vertexShader.Build())
            .SetPixelShader(pixelShader.Build())
            .SetInputLayout(inputLayout.CreateInputLayout())
            .SetBlendState(BlendDescription::CreateNonPremultiplied())
            .SetDepthStencilState(DepthStencilDescription::CreateNone())
            .SetConstantBufferBindSlot("TransformMatrix", 0)
            .Build();
    }

    constantBuffer = std::make_shared<ConstantBuffer>(
        graphicsDevice, sizeof(Matrix4x4), BufferUsage::Dynamic);
}
//-----------------------------------------------------------------------
void LineBatch::Impl::Begin(
    std::shared_ptr<GraphicsCommandList> const& commandListIn,
    Matrix4x4 const& transformMatrix)
{
    POMDOG_ASSERT(commandListIn);
    commandList = commandListIn;

    alignas(16) Matrix4x4 transposedMatrix = Matrix4x4::Transpose(transformMatrix);
    constantBuffer->SetValue(transposedMatrix);
}
//-----------------------------------------------------------------------
void LineBatch::Impl::End()
{
    if (vertices.empty()) {
        return;
    }

    Flush();
}
//-----------------------------------------------------------------------
void LineBatch::Impl::Flush()
{
    POMDOG_ASSERT(!vertices.empty());
    POMDOG_ASSERT(vertices.size() <= MaxVertexCount);
    vertexBuffer->SetData(vertices.data(), vertices.size());

    commandList->SetVertexBuffer(vertexBuffer);
    commandList->SetPipelineState(pipelineState);
    commandList->SetConstantBuffer(0, constantBuffer);
    commandList->SetPrimitiveTopology(PrimitiveTopology::LineList);
    commandList->Draw(vertices.size());

    vertices.clear();
}
//-----------------------------------------------------------------------
void LineBatch::Impl::DrawLine(
    Vector2 const& point1, Vector2 const& point2,
    Vector4 const& color1, Vector4 const& color2)
{
    if (vertices.size() + 2 > MaxVertexCount) {
        Flush();
    }

    POMDOG_ASSERT(vertices.size() + 2 <= Impl::MaxVertexCount);
    vertices.push_back(Vertex{Vector3(point1, 0.0f), color1});
    vertices.push_back(Vertex{Vector3(point2, 0.0f), color2});
}
//-----------------------------------------------------------------------
void LineBatch::Impl::DrawLine(
    Vector3 const& point1, Vector3 const& point2,
    Vector4 const& color1, Vector4 const& color2)
{
    if (vertices.size() + 2 > MaxVertexCount) {
        Flush();
    }

    POMDOG_ASSERT(vertices.size() + 2 <= Impl::MaxVertexCount);
    vertices.push_back(Vertex{point1, color1});
    vertices.push_back(Vertex{point2, color2});
}
//-----------------------------------------------------------------------
void LineBatch::Impl::DrawTriangle(
    Vector2 const& point1, Vector2 const& point2, Vector2 const& point3,
    Vector4 const& color1, Vector4 const& color2, Vector4 const& color3)
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
//-----------------------------------------------------------------------
// MARK: - LineBatch
//-----------------------------------------------------------------------
LineBatch::LineBatch(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
    : impl(std::make_unique<Impl>(graphicsDevice, assets))
{}
//-----------------------------------------------------------------------
LineBatch::~LineBatch() = default;
//-----------------------------------------------------------------------
void LineBatch::Begin(
    std::shared_ptr<GraphicsCommandList> const& commandListIn,
    Matrix4x4 const& transformMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->Begin(commandListIn, transformMatrixIn);
}
//-----------------------------------------------------------------------
void LineBatch::End()
{
    POMDOG_ASSERT(impl);
    impl->End();
}
//-----------------------------------------------------------------------
void LineBatch::DrawBox(BoundingBox const& box, Color const& color)
{
    this->DrawBox(box.Min, box.Max - box.Min, Vector3::Zero, color);
}
//-----------------------------------------------------------------------
void LineBatch::DrawBox(
    Vector3 const& position,
    Vector3 const& scale,
    Color const& color)
{
    this->DrawBox(position, scale, Vector3::Zero, color);
}
//-----------------------------------------------------------------------
void LineBatch::DrawBox(
    Vector3 const& position,
    Vector3 const& scale,
    Vector3 const& originPivot,
    Color const& color)
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

    for (auto & v : boxVertices) {
        v = ((v - originPivot) * scale) + position;
    }

    const auto colorVector = color.ToVector4();
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
//-----------------------------------------------------------------------
void LineBatch::DrawCircle(Vector2 const& position, float radius, Color const& color, std::size_t segments)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(segments >= 3);
    POMDOG_ASSERT(radius >= 0);

    if (radius <= 0) {
        return;
    }

    POMDOG_ASSERT(radius > 0);

    POMDOG_ASSERT(segments >= 3);
    Radian<float> centralAngle = MathConstants<float>::TwoPi() / segments;
    Vector2 prevPoint = position + Vector2{radius, 0};

    auto colorVector = color.ToVector4();

    for (std::size_t i = 0; i < segments; ++i)
    {
        auto rad = centralAngle * (i + 1);
        auto cos = std::cos(rad.value);
        auto sin = std::sin(rad.value);
        auto nextPoint = position + (radius * Vector2{cos, sin});
        impl->DrawLine(nextPoint, prevPoint, colorVector, colorVector);
        prevPoint = nextPoint;
    }
}
//-----------------------------------------------------------------------
void LineBatch::DrawLine(Vector2 const& start, Vector2 const& end, Color const& color)
{
    POMDOG_ASSERT(impl);
    auto colorVector = color.ToVector4();
    impl->DrawLine(start, end, colorVector, colorVector);
}
//-----------------------------------------------------------------------
void LineBatch::DrawLine(Vector2 const& start, Vector2 const& end, Color const& startColor, Color const& endColor)
{
    POMDOG_ASSERT(impl);
    auto colorVector1 = startColor.ToVector4();
    auto colorVector2 = endColor.ToVector4();
    impl->DrawLine(start, end, colorVector1, colorVector2);
}
//-----------------------------------------------------------------------
void LineBatch::DrawLine(Vector3 const& start, Vector3 const& end, Color const& color)
{
    POMDOG_ASSERT(impl);
    auto colorVector = color.ToVector4();
    impl->DrawLine(start, end, colorVector, colorVector);
}
//-----------------------------------------------------------------------
void LineBatch::DrawLine(Vector3 const& start, Vector3 const& end, Color const& startColor, Color const& endColor)
{
    POMDOG_ASSERT(impl);
    auto colorVector1 = startColor.ToVector4();
    auto colorVector2 = endColor.ToVector4();
    impl->DrawLine(start, end, colorVector1, colorVector2);
}
//-----------------------------------------------------------------------
void LineBatch::DrawRectangle(Rectangle const& sourceRect, Color const& color)
{
    POMDOG_ASSERT(impl);
    DrawRectangle(sourceRect, color, color, color, color);
}
//-----------------------------------------------------------------------
void LineBatch::DrawRectangle(Rectangle const& sourceRect,
    Color const& color1, Color const& color2, Color const& color3, Color const& color4)
{
    POMDOG_ASSERT(impl);

    if (sourceRect.Width <= 0 || sourceRect.Height <= 0) {
        return;
    }

    std::array<Vector2, 4> rectVertices = {
        Vector2(sourceRect.GetLeft(), sourceRect.Y - sourceRect.Height),
        Vector2(sourceRect.GetLeft(), sourceRect.Y),
        Vector2(sourceRect.GetRight(), sourceRect.Y),
        Vector2(sourceRect.GetRight(), sourceRect.Y - sourceRect.Height),
    };

    auto colorVector1 = color1.ToVector4();
    auto colorVector2 = color2.ToVector4();
    auto colorVector3 = color3.ToVector4();
    auto colorVector4 = color4.ToVector4();

    impl->DrawLine(rectVertices[0], rectVertices[1], colorVector1, colorVector2);
    impl->DrawLine(rectVertices[1], rectVertices[2], colorVector2, colorVector3);
    impl->DrawLine(rectVertices[2], rectVertices[3], colorVector3, colorVector4);
    impl->DrawLine(rectVertices[3], rectVertices[0], colorVector4, colorVector1);
}
//-----------------------------------------------------------------------
void LineBatch::DrawRectangle(Matrix3x2 const& matrix,
    Rectangle const& sourceRect, Color const& color)
{
    POMDOG_ASSERT(impl);

    if (sourceRect.Width <= 0 || sourceRect.Height <= 0) {
        return;
    }

    std::array<Vector2, 4> rectVertices = {
        Vector2(sourceRect.GetLeft(), sourceRect.Y - sourceRect.Height),
        Vector2(sourceRect.GetLeft(), sourceRect.Y),
        Vector2(sourceRect.GetRight(), sourceRect.Y),
        Vector2(sourceRect.GetRight(), sourceRect.Y - sourceRect.Height),
    };

    for (auto & vertex: rectVertices) {
        vertex = Vector2::Transform(vertex, matrix);
    }

    auto colorVector = color.ToVector4();

    impl->DrawLine(rectVertices[0], rectVertices[1], colorVector, colorVector);
    impl->DrawLine(rectVertices[1], rectVertices[2], colorVector, colorVector);
    impl->DrawLine(rectVertices[2], rectVertices[3], colorVector, colorVector);
    impl->DrawLine(rectVertices[3], rectVertices[0], colorVector, colorVector);
}
//-----------------------------------------------------------------------
void LineBatch::DrawSphere(
    Vector3 const& position,
    float radius,
    Color const& color,
    std::size_t segments)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(segments >= 4);
    POMDOG_ASSERT(radius >= 0);

    if (radius <= 0) {
        return;
    }

    const auto rings = std::max(static_cast<int>(segments), 4);
    const auto sectors = std::max(static_cast<int>(segments), 4);

    POMDOG_ASSERT(sectors > 0);
    POMDOG_ASSERT(rings > 0);

    std::vector<Vector3> sphereVertices;
    sphereVertices.reserve(sectors * (rings - 1) + 2);

    const auto R = 1.0f / static_cast<float>(rings - 1);
    const auto S = 1.0f / static_cast<float>(sectors - 1);

    // Create sphere vertices
    sphereVertices.push_back(Vector3{0.0f, 1.0f, 0.0f});
    for (int ring = 1; ring < rings; ++ring) {
        const auto latitude = MathConstants<float>::Pi() * ring * R;
        const auto y = std::cos(latitude);
        const auto r = std::sin(latitude);
        for (int s = 0; s < sectors; ++s) {
            auto longitude = MathConstants<float>::TwoPi() * s * S;
            auto x = r * std::cos(longitude);
            auto z = r * std::sin(longitude);
            sphereVertices.push_back(Vector3{x, y, z});
        }
    }
    sphereVertices.push_back(Vector3{0.0f, -1.0f, 0.0f});

    // Scaling and translation
    for (auto & v : sphereVertices) {
        v = v * radius + position;
    }

    const auto colorVector = color.ToVector4();
    const auto drawIndices = [&](std::size_t a, std::size_t b) {
        POMDOG_ASSERT(a < sphereVertices.size());
        POMDOG_ASSERT(b < sphereVertices.size());
        auto const& start = sphereVertices[a];
        auto const& end = sphereVertices[b];
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
//-----------------------------------------------------------------------
void LineBatch::DrawTriangle(Vector2 const& point1, Vector2 const& point2, Vector2 const& point3, Color const& color)
{
    POMDOG_ASSERT(impl);
    auto colorVector = color.ToVector4();
    impl->DrawTriangle(point1, point2, point3, colorVector, colorVector, colorVector);
}
//-----------------------------------------------------------------------
void LineBatch::DrawTriangle(
    Vector2 const& point1, Vector2 const& point2, Vector2 const& point3,
    Color const& color1, Color const& color2, Color const& color3)
{
    POMDOG_ASSERT(impl);
    auto colorVector1 = color1.ToVector4();
    auto colorVector2 = color2.ToVector4();
    auto colorVector3 = color3.ToVector4();
    impl->DrawTriangle(point1, point2, point3, colorVector1, colorVector2, colorVector3);
}
//-----------------------------------------------------------------------
} // namespace Pomdog
