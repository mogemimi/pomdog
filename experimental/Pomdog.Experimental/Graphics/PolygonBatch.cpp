// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "PolygonBatch.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/ConstantBufferBinding.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include <cmath>

namespace Pomdog {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/LineBatch_VS.inc.hpp"
#include "Shaders/GLSL.Embedded/LineBatch_PS.inc.hpp"

}// unnamed namespace
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - PolygonBatch::Impl
#endif
//-----------------------------------------------------------------------
class PolygonBatch::Impl {
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
    std::shared_ptr<GraphicsContext> graphicsContext;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<ConstantBufferBinding> constantBuffers;

public:
    std::uint32_t drawCallCount;

public:
    Impl(std::shared_ptr<GraphicsContext> const& graphicsContext,
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets);

    void Begin(Matrix4x4 const& transformMatrix);

    void DrawTriangle(Vector2 const& point1, Vector2 const& point2, Vector2 const& point3,
        Vector4 const& color1, Vector4 const& color2, Vector4 const& color3);

    void End();

    void Flush();
};
//-----------------------------------------------------------------------
PolygonBatch::Impl::Impl(std::shared_ptr<GraphicsContext> const& graphicsContextIn,
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
    : graphicsContext(graphicsContextIn)
    , drawCallCount(0)
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

        auto vertexShader = assets.CreateBuilder<Shader>()
            .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::VertexShader)
            .SetGLSL(Builtin_GLSL_LineBatch_VS, std::strlen(Builtin_GLSL_LineBatch_VS));

        auto pixelShader = assets.CreateBuilder<Shader>()
            .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::PixelShader)
            .SetGLSL(Builtin_GLSL_LineBatch_PS, std::strlen(Builtin_GLSL_LineBatch_PS));

        auto builder = assets.CreateBuilder<PipelineState>();
        pipelineState = builder
            .SetVertexShader(vertexShader.Build())
            .SetPixelShader(pixelShader.Build())
            .SetInputLayout(inputLayout.CreateInputLayout())
            .SetBlendState(BlendDescription::CreateNonPremultiplied())
            .SetDepthStencilState(DepthStencilDescription::CreateNone())
            .Build();

        constantBuffers = builder.CreateConstantBuffers(pipelineState);
    }
}
//-----------------------------------------------------------------------
void PolygonBatch::Impl::Begin(Matrix4x4 const& transformMatrix)
{
    alignas(16) Matrix4x4 transposedMatrix = Matrix4x4::Transpose(transformMatrix);

    auto parameter = constantBuffers->Find("TransformMatrix");
    parameter->SetValue(transposedMatrix);

    drawCallCount = 0;
}
//-----------------------------------------------------------------------
void PolygonBatch::Impl::End()
{
    if (vertices.empty()) {
        return;
    }

    Flush();
}
//-----------------------------------------------------------------------
void PolygonBatch::Impl::Flush()
{
    POMDOG_ASSERT(!vertices.empty());
    POMDOG_ASSERT(vertices.size() <= MaxVertexCount);
    vertexBuffer->SetData(vertices.data(), vertices.size());

    graphicsContext->SetVertexBuffer(vertexBuffer);
    graphicsContext->SetPipelineState(pipelineState);
    graphicsContext->SetConstantBuffers(constantBuffers);
    graphicsContext->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
    graphicsContext->Draw(vertices.size());

    ++drawCallCount;

    vertices.clear();
}
//-----------------------------------------------------------------------
void PolygonBatch::Impl::DrawTriangle(Vector2 const& point1, Vector2 const& point2, Vector2 const& point3,
    Vector4 const& color1, Vector4 const& color2, Vector4 const& color3)
{
    if (vertices.size() + 3 > MaxVertexCount) {
        Flush();
    }

    POMDOG_ASSERT(vertices.size() + 3 <= Impl::MaxVertexCount);
    vertices.push_back(Vertex{Vector3(point1, 0.0f), color1});
    vertices.push_back(Vertex{Vector3(point2, 0.0f), color2});
    vertices.push_back(Vertex{Vector3(point3, 0.0f), color3});
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - PolygonBatch
#endif
//-----------------------------------------------------------------------
PolygonBatch::PolygonBatch(std::shared_ptr<GraphicsContext> const& graphicsContext,
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
    : impl(std::make_unique<Impl>(graphicsContext, graphicsDevice, assets))
{}
//-----------------------------------------------------------------------
PolygonBatch::~PolygonBatch() = default;
//-----------------------------------------------------------------------
void PolygonBatch::Begin(Matrix4x4 const& transformMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->Begin(transformMatrixIn);
}
//-----------------------------------------------------------------------
void PolygonBatch::End()
{
    POMDOG_ASSERT(impl);
    impl->End();
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawCircle(Vector2 const& position, float radius, Color const& color, std::size_t segments)
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
        impl->DrawTriangle(nextPoint, prevPoint, position, colorVector, colorVector, colorVector);
        prevPoint = nextPoint;
    }
}
//-----------------------------------------------------------------------
//void PolygonBatch::DrawPolyline(std::vector<Vector2> const& points, float thickness, Color const& color)
//{
//    POMDOG_ASSERT(impl);
//    POMDOG_ASSERT(!points.empty());
//    POMDOG_ASSERT(points.size() >= 2);
//    POMDOG_ASSERT(thickness >= 0);
//
//    if (thickness <= 0) {
//        return;
//    }
//
//    auto prevPoint = points.front();
//    for (auto iter = std::next(std::begin(points)); iter < std::end(points); ++iter)
//    {
//        auto & origin = *iter;
//        auto tangent = origin - prevPoint;
//        auto normal = Vector2{-tangent.Y, tangent.X};
//
//        auto p1 = prevPoint + (thickness * normal);
//        auto p2 = prevPoint + (-thickness * normal);
//
//        Vector2 nextPoint = *iter;
//        auto nextIter = std::next(iter);
//        if (nextIter != std::end(points))
//        {
//            nextPoint = *nextIter;
//
//            // TODO
//            // ...
//        }
//        else
//        {
//            auto p3 = origin + (thickness * normal);
//            auto p4 = origin + (-thickness * normal);
//            impl->DrawTriangle(p1, p2, p3, color, color, color);
//            impl->DrawTriangle(p3, p4, p1, color, color, color);
//        }
//    }
//}
//-----------------------------------------------------------------------
void PolygonBatch::DrawRectangle(Rectangle const& sourceRect, Color const& color)
{
    POMDOG_ASSERT(impl);
    DrawRectangle(sourceRect, color, color, color, color);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawRectangle(Rectangle const& sourceRect,
    Color const& color1, Color const& color2, Color const& color3, Color const& color4)
{
    POMDOG_ASSERT(impl);

    if (sourceRect.Width <= 0 || sourceRect.Height <= 0) {
        return;
    }

    std::array<Vector2, 4> rectVertices = {
        Vector2(sourceRect.GetLeft(), sourceRect.Y),
        Vector2(sourceRect.GetLeft(), sourceRect.Y + sourceRect.Height),
        Vector2(sourceRect.GetRight(), sourceRect.Y + sourceRect.Height),
        Vector2(sourceRect.GetRight(), sourceRect.Y),
    };

    auto colorVector1 = color1.ToVector4();
    auto colorVector2 = color2.ToVector4();
    auto colorVector3 = color3.ToVector4();
    auto colorVector4 = color4.ToVector4();

    impl->DrawTriangle(rectVertices[0], rectVertices[1], rectVertices[2],
        colorVector1, colorVector4, colorVector3);
    impl->DrawTriangle(rectVertices[2], rectVertices[3], rectVertices[0],
        colorVector3, colorVector2, colorVector1);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawRectangle(Matrix3x2 const& matrix,
    Rectangle const& sourceRect, Color const& color)
{
    POMDOG_ASSERT(impl);
    DrawRectangle(matrix, sourceRect, color, color, color, color);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawRectangle(Matrix3x2 const& matrix,
    Rectangle const& sourceRect,
    Color const& color1, Color const& color2, Color const& color3, Color const& color4)
{
    POMDOG_ASSERT(impl);

    if (sourceRect.Width <= 0 || sourceRect.Height <= 0) {
        return;
    }

    std::array<Vector2, 4> rectVertices = {
        Vector2(sourceRect.GetLeft(), sourceRect.Y),
        Vector2(sourceRect.GetLeft(), sourceRect.Y + sourceRect.Height),
        Vector2(sourceRect.GetRight(), sourceRect.Y + sourceRect.Height),
        Vector2(sourceRect.GetRight(), sourceRect.Y),
    };

    for (auto & vertex: rectVertices) {
        vertex = Vector2::Transform(vertex, matrix);
    }

    auto colorVector1 = color1.ToVector4();
    auto colorVector2 = color2.ToVector4();
    auto colorVector3 = color3.ToVector4();
    auto colorVector4 = color4.ToVector4();

    impl->DrawTriangle(rectVertices[0], rectVertices[1], rectVertices[2],
        colorVector1, colorVector4, colorVector3);
    impl->DrawTriangle(rectVertices[2], rectVertices[3], rectVertices[0],
        colorVector3, colorVector2, colorVector1);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawTriangle(Vector2 const& point1, Vector2 const& point2, Vector2 const& point3, Color const& color)
{
    POMDOG_ASSERT(impl);
    auto colorVector = color.ToVector4();
    impl->DrawTriangle(point1, point2, point3, colorVector, colorVector, colorVector);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawTriangle(Vector2 const& point1, Vector2 const& point2, Vector2 const& point3,
    Color const& color1, Color const& color2, Color const& color3)
{
    POMDOG_ASSERT(impl);
    auto colorVector1 = color1.ToVector4();
    auto colorVector2 = color2.ToVector4();
    auto colorVector3 = color3.ToVector4();
    impl->DrawTriangle(point1, point2, point3, colorVector1, colorVector2, colorVector3);
}
//-----------------------------------------------------------------------
std::uint32_t PolygonBatch::DrawCallCount() const
{
    POMDOG_ASSERT(impl);
    return impl->drawCallCount;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
