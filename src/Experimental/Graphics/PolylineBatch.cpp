// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Graphics/PolylineBatch.hpp"
#include "../../Utility/AlignedNew.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
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
#include <cstring>

#include "Pomdog/Graphics/RasterizerDescription.hpp"

using Pomdog::Detail::AlignedNew;

namespace Pomdog {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/PolylineBatch_PS.inc.hpp"
#include "Shaders/GLSL.Embedded/PolylineBatch_VS.inc.hpp"
#include "Shaders/HLSL.Embedded/PolylineBatch_PS.inc.hpp"
#include "Shaders/HLSL.Embedded/PolylineBatch_VS.inc.hpp"
#include "Shaders/Metal.Embedded/PolylineBatch.inc.hpp"

struct PolylineBatchVertex final {
    Vector3 Position;
    Pomdog::Color Color;
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
    float extrusion,
    float thickness)
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

} // unnamed namespace

// MARK: - PolylineBatch::Impl

class PolylineBatch::Impl {
public:
    static constexpr std::size_t MaxVertexCount = 8192;
    static constexpr std::size_t MinVertexCount = 256;
    static constexpr std::size_t MinIndexCount = (MinVertexCount - 2) * 6;
    static constexpr std::size_t MaxIndexCount = (MaxVertexCount - 2) * 6;

private:
    std::shared_ptr<GraphicsCommandList> commandList;
    std::shared_ptr<VertexBuffer> vertexBuffer;
#ifdef POMDOG_POLYLINE_DEBUG
    std::shared_ptr<VertexBuffer> debugVertexBuffer;
#endif
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<ConstantBuffer> constantBuffer;

    std::vector<PolylineVertex> vertices;
    std::vector<std::uint16_t> indices;
    std::size_t startIndexLocation = 0;

public:
    Impl(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager & assets);

    void Begin(
        const std::shared_ptr<GraphicsCommandList>& commandListIn,
        const Matrix4x4& transformMatrix);

    void DrawPath(std::vector<PolylineBatchVertex>&& path, bool closed, float thickness);

    void End();

    void Flush();
};

PolylineBatch::Impl::Impl(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager & assets)
{
    {
        vertices.reserve(MinVertexCount);

        constexpr auto maxVertexCount = MaxVertexCount;
        vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
            maxVertexCount, sizeof(PolylineVertex), BufferUsage::Dynamic);

#ifdef POMDOG_POLYLINE_DEBUG
        debugVertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
            maxVertexCount, sizeof(PolylineVertex), BufferUsage::Dynamic);
#endif
    }
    {
        indices.reserve(MinIndexCount);

        constexpr auto maxIndexCount = MaxIndexCount;
        indexBuffer = std::make_shared<IndexBuffer>(graphicsDevice,
            IndexElementSize::SixteenBits, maxIndexCount, BufferUsage::Dynamic);
    }
    {
        auto inputLayout = InputLayoutHelper{}
            .Float4().Float4().Float4().Float4();

        auto vertexShader = assets.CreateBuilder<Shader>(ShaderPipelineStage::VertexShader)
            .SetGLSL(Builtin_GLSL_PolylineBatch_VS, std::strlen(Builtin_GLSL_PolylineBatch_VS))
            .SetHLSLPrecompiled(BuiltinHLSL_PolylineBatch_VS, sizeof(BuiltinHLSL_PolylineBatch_VS))
            .SetMetal(Builtin_Metal_PolylineBatch, std::strlen(Builtin_Metal_PolylineBatch), "PolylineBatchVS");

        auto pixelShader = assets.CreateBuilder<Shader>(ShaderPipelineStage::PixelShader)
            .SetGLSL(Builtin_GLSL_PolylineBatch_PS, std::strlen(Builtin_GLSL_PolylineBatch_PS))
            .SetHLSLPrecompiled(BuiltinHLSL_PolylineBatch_PS, sizeof(BuiltinHLSL_PolylineBatch_PS))
            .SetMetal(Builtin_Metal_PolylineBatch, std::strlen(Builtin_Metal_PolylineBatch), "PolylineBatchPS");

        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        pipelineState = assets.CreateBuilder<PipelineState>()
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetVertexShader(vertexShader.Build())
            .SetPixelShader(pixelShader.Build())
            .SetInputLayout(inputLayout.CreateInputLayout())
            .SetBlendState(BlendDescription::CreateNonPremultiplied())
            .SetDepthStencilState(DepthStencilDescription::CreateDefault())
            .SetRasterizerState(RasterizerDescription::CreateCullNone())
            .SetConstantBufferBindSlot("TransformMatrix", 0)
            .Build();
    }

    constantBuffer = std::make_shared<ConstantBuffer>(
        graphicsDevice, sizeof(Matrix4x4), BufferUsage::Dynamic);
}

void PolylineBatch::Impl::Begin(
    const std::shared_ptr<GraphicsCommandList>& commandListIn,
    const Matrix4x4& transformMatrix)
{
    POMDOG_ASSERT(commandListIn);
    commandList = commandListIn;

    alignas(16) Matrix4x4 transposedMatrix = Matrix4x4::Transpose(transformMatrix);
    constantBuffer->SetValue(transposedMatrix);
}

void PolylineBatch::Impl::End()
{
    if (vertices.empty()) {
        return;
    }

    Flush();
    commandList.reset();
    vertices.clear();
    indices.clear();
    startIndexLocation = 0;
}

void PolylineBatch::Impl::Flush()
{
#if 1
    if (vertices.size() >= 2) {
        vertices[vertices.size() - 1].Color.W *= 0.1f;
        vertices[vertices.size() - 2].Color.W *= 0.1f;
    }
#endif

    POMDOG_ASSERT(!vertices.empty());
    POMDOG_ASSERT(vertices.size() <= MaxVertexCount);
    vertexBuffer->SetData(vertices.data(), vertices.size());

    POMDOG_ASSERT(!indices.empty());
    POMDOG_ASSERT(indices.size() <= MaxIndexCount);
    indexBuffer->SetData(indices.data(), indices.size());

#ifdef POMDOG_POLYLINE_DEBUG
    auto vert = vertices;
    for (auto& v : vert) {
        v.Color = Vector4{0.0f, 0.0f, 0.0f, v.Color.W * 0.5f};
    }
    debugVertexBuffer->SetData(vert.data(), vert.size());
#endif

    commandList->SetVertexBuffer(vertexBuffer);
    commandList->SetPipelineState(pipelineState);
    commandList->SetConstantBuffer(0, constantBuffer);
    commandList->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
    commandList->DrawIndexed(indexBuffer, indices.size(), startIndexLocation);

#ifdef POMDOG_POLYLINE_DEBUG
    commandList->SetVertexBuffer(debugVertexBuffer);
    commandList->SetPrimitiveTopology(PrimitiveTopology::LineStrip);
    commandList->DrawIndexed(indexBuffer, indices.size(), startIndexLocation);
#endif

    startIndexLocation = indices.size();
}

void PolylineBatch::Impl::DrawPath(std::vector<PolylineBatchVertex>&& path, bool closed, float thickness)
{
    const auto n = static_cast<int>(path.size());

    if ((vertices.size() + path.size() * 2) > MaxVertexCount) {
        Flush();
        return;
    }

    if (n < 2) {
        return;
    }

    for (int i = 0; i < n; i++) {
        POMDOG_ASSERT(n >= 2);
        const auto& v = path[i];
        auto color = v.Color.ToVector4();

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

                if (Vector3::DistanceSquared(v.Position, nextPoint) < 0.0001f) {
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

                if (Vector3::DistanceSquared(v.Position, nextPoint) < 0.0001f) {
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

            if (Vector3::DistanceSquared(v.Position, nextPoint) < 0.0001f) {
                auto dir = v.Position - start;
                nextPoint = v.Position + dir;
            }
        }

        if (i < (n - 1) || closed) {
            // NOTE: shape corner
            constexpr float cutoff = -0.90f;
            const auto v1 = Vector3::Normalize(v.Position - prevPoint);
            const auto v2 = Vector3::Normalize(nextPoint - v.Position);
            if (Vector3::Dot(v2, v1) < cutoff) {
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

PolylineBatch::PolylineBatch(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager & assets)
    : impl(std::make_unique<Impl>(graphicsDevice, assets))
{
}

PolylineBatch::~PolylineBatch() = default;

void PolylineBatch::Begin(
    const std::shared_ptr<GraphicsCommandList>& commandListIn,
    const Matrix4x4& transformMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->Begin(commandListIn, transformMatrixIn);
}

void PolylineBatch::End()
{
    POMDOG_ASSERT(impl);
    impl->End();
}

void PolylineBatch::DrawPath(const std::vector<Vector2>& path, bool closed, const Color& color, float thickness)
{
    POMDOG_ASSERT(impl);
    std::vector<PolylineBatchVertex> vertices;
    for (const auto& v : path) {
        vertices.push_back(PolylineBatchVertex{Vector3{v, 0.0f}, color});
    }
    if (vertices.size() <= 2) {
        closed = false;
    }
    impl->DrawPath(std::move(vertices), closed, thickness);
}

void PolylineBatch::DrawBox(const BoundingBox& box, const Color& color, float thickness)
{
    this->DrawBox(box.Min, box.Max - box.Min, Vector3::Zero, color, thickness);
}

void PolylineBatch::DrawBox(
    const Vector3& position,
    const Vector3& scale,
    const Color& color,
    float thickness)
{
    this->DrawBox(position, scale, Vector3::Zero, color, thickness);
}

void PolylineBatch::DrawBox(
    const Vector3& position,
    const Vector3& scale,
    const Vector3& originPivot,
    const Color& color,
    float thickness)
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
        this->DrawLine(boxVertices[a], boxVertices[b], color, color, thickness);
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

void PolylineBatch::DrawCircle(const Vector2& position, float radius, const Color& color, int segments, float thickness)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(segments >= 3);
    POMDOG_ASSERT(radius >= 0);

    if (radius <= 0) {
        return;
    }

    POMDOG_ASSERT(radius > 0);

    POMDOG_ASSERT(segments >= 3);
    Radian<float> centralAngle = Math::TwoPi<float> / segments;
    Vector2 prevPoint = position + Vector2{radius, 0};

    for (int i = 0; i < segments; ++i) {
        auto rad = centralAngle * static_cast<float>(i + 1);
        auto cos = std::cos(rad.value);
        auto sin = std::sin(rad.value);
        auto nextPoint = position + (radius * Vector2{cos, sin});
        this->DrawLine(nextPoint, prevPoint, color, color, thickness);
        prevPoint = nextPoint;
    }
}

void PolylineBatch::DrawLine(const Vector2& start, const Vector2& end, const Color& color, float thickness)
{
    POMDOG_ASSERT(impl);
    impl->DrawPath(std::vector<PolylineBatchVertex>{
        PolylineBatchVertex{Vector3{start, 0.0f}, color},
        PolylineBatchVertex{Vector3{end, 0.0f}, color},
    }, false, thickness);
}

void PolylineBatch::DrawLine(const Vector2& start, const Vector2& end, const Color& startColor, const Color& endColor, float thickness)
{
    POMDOG_ASSERT(impl);
    impl->DrawPath(std::vector<PolylineBatchVertex>{
        PolylineBatchVertex{Vector3{start, 0.0f}, startColor},
        PolylineBatchVertex{Vector3{end, 0.0f}, endColor},
    }, false, thickness);
}

void PolylineBatch::DrawLine(const Vector3& start, const Vector3& end, const Color& color, float thickness)
{
    POMDOG_ASSERT(impl);
    impl->DrawPath(std::vector<PolylineBatchVertex>{
        PolylineBatchVertex{start, color},
        PolylineBatchVertex{end, color},
    }, false, thickness);
}

void PolylineBatch::DrawLine(const Vector3& start, const Vector3& end, const Color& startColor, const Color& endColor, float thickness)
{
    POMDOG_ASSERT(impl);
    impl->DrawPath(std::vector<PolylineBatchVertex>{
        PolylineBatchVertex{start, startColor},
        PolylineBatchVertex{end, endColor},
    }, false, thickness);
}

void PolylineBatch::DrawRectangle(const Rectangle& sourceRect, const Color& color, float thickness)
{
    POMDOG_ASSERT(impl);
    DrawRectangle(sourceRect, color, color, color, color, thickness);
}

void PolylineBatch::DrawRectangle(const Rectangle& sourceRect,
    const Color& color1, const Color& color2, const Color& color3, const Color& color4, float thickness)
{
    POMDOG_ASSERT(impl);

    if (sourceRect.Width <= 0 || sourceRect.Height <= 0) {
        return;
    }

    std::array<Vector2, 4> rectVertices = {{
        Vector2{static_cast<float>(sourceRect.GetLeft()), static_cast<float>(sourceRect.Y - sourceRect.Height)},
        Vector2{static_cast<float>(sourceRect.GetLeft()), static_cast<float>(sourceRect.Y)},
        Vector2{static_cast<float>(sourceRect.GetRight()), static_cast<float>(sourceRect.Y)},
        Vector2{static_cast<float>(sourceRect.GetRight()), static_cast<float>(sourceRect.Y - sourceRect.Height)},
    }};

    this->DrawLine(rectVertices[0], rectVertices[1], color1, color2, thickness);
    this->DrawLine(rectVertices[1], rectVertices[2], color2, color3, thickness);
    this->DrawLine(rectVertices[2], rectVertices[3], color3, color4, thickness);
    this->DrawLine(rectVertices[3], rectVertices[0], color4, color1, thickness);
}

void PolylineBatch::DrawRectangle(const Matrix3x2& matrix,
    const Rectangle& sourceRect, const Color& color, float thickness)
{
    POMDOG_ASSERT(impl);

    if (sourceRect.Width <= 0 || sourceRect.Height <= 0) {
        return;
    }

    std::array<Vector2, 4> rectVertices = {{
        Vector2{static_cast<float>(sourceRect.GetLeft()), static_cast<float>(sourceRect.Y - sourceRect.Height)},
        Vector2{static_cast<float>(sourceRect.GetLeft()), static_cast<float>(sourceRect.Y)},
        Vector2{static_cast<float>(sourceRect.GetRight()), static_cast<float>(sourceRect.Y)},
        Vector2{static_cast<float>(sourceRect.GetRight()), static_cast<float>(sourceRect.Y - sourceRect.Height)},
    }};

    for (auto & vertex: rectVertices) {
        vertex = Vector2::Transform(vertex, matrix);
    }

    this->DrawLine(rectVertices[0], rectVertices[1], color, color, thickness);
    this->DrawLine(rectVertices[1], rectVertices[2], color, color, thickness);
    this->DrawLine(rectVertices[2], rectVertices[3], color, color, thickness);
    this->DrawLine(rectVertices[3], rectVertices[0], color, color, thickness);
}

void PolylineBatch::DrawSphere(
    const Vector3& position,
    float radius,
    const Color& color,
    int segments,
    float thickness)
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
        const auto latitude = Math::Pi<float> * ring * R;
        const auto y = std::cos(latitude);
        const auto r = std::sin(latitude);
        for (int s = 0; s < sectors; ++s) {
            auto longitude = Math::TwoPi<float> * s * S;
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

    const auto drawIndices = [&](std::size_t a, std::size_t b) {
        POMDOG_ASSERT(a < sphereVertices.size());
        POMDOG_ASSERT(b < sphereVertices.size());
        const auto& start = sphereVertices[a];
        const auto& end = sphereVertices[b];
        this->DrawLine(start, end, color, color, thickness);
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

        impl->DrawPath(std::move(path1), false, thickness);

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

void PolylineBatch::DrawTriangle(const Vector2& point1, const Vector2& point2, const Vector2& point3, const Color& color, float thickness)
{
    POMDOG_ASSERT(impl);

    impl->DrawPath(std::vector<PolylineBatchVertex>{
        PolylineBatchVertex{Vector3{point1, 0.0f}, color},
        PolylineBatchVertex{Vector3{point2, 0.0f}, color},
        PolylineBatchVertex{Vector3{point2, 0.0f}, color},
        PolylineBatchVertex{Vector3{point3, 0.0f}, color},
        PolylineBatchVertex{Vector3{point3, 0.0f}, color},
        PolylineBatchVertex{Vector3{point1, 0.0f}, color},
    }, false, thickness);
}

void PolylineBatch::DrawTriangle(
    const Vector2& point1, const Vector2& point2, const Vector2& point3,
    const Color& color1, const Color& color2, const Color& color3,
    float thickness)
{
    POMDOG_ASSERT(impl);
    impl->DrawPath(std::vector<PolylineBatchVertex>{
        PolylineBatchVertex{Vector3{point1, 0.0f}, color1},
        PolylineBatchVertex{Vector3{point2, 0.0f}, color2},
        PolylineBatchVertex{Vector3{point2, 0.0f}, color2},
        PolylineBatchVertex{Vector3{point3, 0.0f}, color3},
        PolylineBatchVertex{Vector3{point3, 0.0f}, color3},
        PolylineBatchVertex{Vector3{point1, 0.0f}, color1},
    }, false, thickness);
}

} // namespace Pomdog
