// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "SpriteRenderer.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/IndexElementSize.hpp"
#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include <tuple>

namespace Pomdog {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/SpriteRenderer_VS.inc.hpp"
#include "Shaders/GLSL.Embedded/SpriteRenderer_PS.inc.hpp"
#include "Shaders/HLSL.Embedded/SpriteRenderer_VS.inc.hpp"
#include "Shaders/HLSL.Embedded/SpriteRenderer_PS.inc.hpp"

}// unnamed namespace
//-----------------------------------------------------------------------
// MARK: - SpriteRenderer::Impl
//-----------------------------------------------------------------------
class SpriteRenderer::Impl {
private:
    static constexpr std::size_t MaxBatchSize = 2048;
    static constexpr std::size_t MinBatchSize = 128;

    static_assert(MaxBatchSize >= MinBatchSize, "");

    struct alignas(16) SpriteInfo
    {
        // {x___} = worldMatrix.M00
        // {_y__} = worldMatrix.M01
        // {__z_} = worldMatrix.M10
        // {___w} = worldMatrix.M11
        Vector4 TransformMatrix1;

        // {x___} = worldMatrix.M20
        // {_y__} = worldMatrix.M21
        // {__z_} = layerDepth
        // {___w} = rotation
        Vector4 TransformMatrix2;

        // {xy__} = xy
        // {__zw} = {width, height}
        Vector4 SourceRect;

        // {xy__} = originPivot.xy
        // {__zw} = scale.xy
        Vector4 OriginScale;

        // {rgb_} = color.rgb
        // {___a} = color.a
        Vector4 Color;
    };

private:
    std::shared_ptr<GraphicsContext> graphicsContext;

    std::vector<std::vector<SpriteInfo>> spriteQueues;
    std::vector<std::shared_ptr<Texture2D>> textures;

    std::shared_ptr<VertexBuffer> planeVertices;
    std::shared_ptr<IndexBuffer> planeIndices;
    std::shared_ptr<VertexBuffer> instanceVertices;

    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<ConstantBufferBinding> constantBuffers;

    Matrix4x4 projectionMatrix;
    SpriteSortMode sortMode;

public:
    Impl(
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets);

    void ResetProjectionMatrix(Matrix4x4 const& projectionMatrix);

    void Begin(SpriteSortMode sortMode);

    void Begin(SpriteSortMode sortMode, Matrix4x4 const& transformMatrix);

    void Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
        Vector2 const& position, Color const& color,
        Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth);

    void Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
        Vector2 const& position, Rectangle const& sourceRect, Color const& color,
        Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth);

    void End();

private:
    void Flush();
    void DrawInstance(std::shared_ptr<Texture2D> const& texture, std::vector<SpriteInfo> const& sprites);
};
//-----------------------------------------------------------------------
SpriteRenderer::Impl::Impl(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
    : graphicsContext(graphicsContextIn)
    , sortMode(SpriteSortMode::BackToFront)
{
    {
        auto viewport = graphicsContext->GetViewport();
        POMDOG_ASSERT(viewport.Width > 0);
        POMDOG_ASSERT(viewport.Height > 0);
        projectionMatrix = Matrix4x4::CreateOrthographicLH(viewport.Width, viewport.Height, 0.1f, 100.0f);
    }
    {
        using PositionTextureCoord = Vector4;
        std::array<PositionTextureCoord, 4> const verticesCombo = {
            Vector4(0.0f, 0.0f, 0.0f, 1.0f),
            Vector4(0.0f, 1.0f, 0.0f, 0.0f),
            Vector4(1.0f, 1.0f, 1.0f, 0.0f),
            Vector4(1.0f, 0.0f, 1.0f, 1.0f),
        };
        planeVertices = std::make_shared<VertexBuffer>(graphicsDevice,
            verticesCombo.data(), verticesCombo.size(),
            sizeof(PositionTextureCoord), BufferUsage::Immutable);
    }
    {
        std::array<std::uint16_t, 6> const indices = {
            0, 1, 2,
            2, 3, 0
        };

        // Create index buffer
        planeIndices = std::make_shared<IndexBuffer>(graphicsDevice,
            IndexElementSize::SixteenBits, indices.data(), indices.size(), BufferUsage::Immutable);
    }
    {
        auto maxBatchSize = MaxBatchSize;
        instanceVertices = std::make_shared<VertexBuffer>(graphicsDevice,
            maxBatchSize, sizeof(SpriteInfo), BufferUsage::Dynamic);
    }
    {
        auto inputLayout = InputLayoutHelper{}
            .AddInputSlot()
            .Float4()
            .AddInputSlot(InputClassification::InputPerInstance, 1)
            .Float4().Float4().Float4().Float4().Float4();

        auto vertexShader = assets.CreateBuilder<Shader>()
            .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::VertexShader)
            .SetGLSL(Builtin_GLSL_SpriteRenderer_VS, std::strlen(Builtin_GLSL_SpriteRenderer_VS))
            .SetHLSLPrecompiled(BuiltinHLSL_SpriteRenderer_VS, sizeof(BuiltinHLSL_SpriteRenderer_VS));

        auto pixelShader = assets.CreateBuilder<Shader>()
            .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::PixelShader)
            .SetGLSL(Builtin_GLSL_SpriteRenderer_PS, std::strlen(Builtin_GLSL_SpriteRenderer_PS))
            .SetHLSLPrecompiled(BuiltinHLSL_SpriteRenderer_PS, sizeof(BuiltinHLSL_SpriteRenderer_PS));

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
void SpriteRenderer::Impl::ResetProjectionMatrix(Matrix4x4 const& projectionMatrixIn)
{
    this->projectionMatrix = projectionMatrixIn;
}
//-----------------------------------------------------------------------
void SpriteRenderer::Impl::Begin(SpriteSortMode sortModeIn)
{
    this->sortMode = sortModeIn;

    alignas(16) Matrix4x4 projection = Matrix4x4::Transpose(projectionMatrix);

    auto parameter = constantBuffers->Find("Matrices");
    parameter->SetValue(projection);
}
//-----------------------------------------------------------------------
void SpriteRenderer::Impl::Begin(SpriteSortMode sortModeIn, Matrix4x4 const& transformMatrix)
{
    this->sortMode = sortModeIn;

    alignas(16) Matrix4x4 projection = Matrix4x4::Transpose(
        transformMatrix * projectionMatrix);

    auto contantBuffer = constantBuffers->Find("Matrices");
    contantBuffer->SetValue(projection);
}
//-----------------------------------------------------------------------
void SpriteRenderer::Impl::End()
{
    Flush();
}
//-----------------------------------------------------------------------
void SpriteRenderer::Impl::Flush()
{
    if (textures.empty()) {
        return;
    }

    for (std::size_t queuePos = 0; queuePos < textures.size(); ++queuePos)
    {
        POMDOG_ASSERT(!spriteQueues.empty());
        POMDOG_ASSERT(queuePos < spriteQueues.size());
        POMDOG_ASSERT(!spriteQueues[queuePos].empty());

        auto & sprites = spriteQueues[queuePos];

        switch (sortMode) {
        case SpriteSortMode::BackToFront:
            std::sort(std::begin(sprites), std::end(sprites), [](SpriteInfo const& a, SpriteInfo const& b) {
                return a.TransformMatrix2.Z > b.TransformMatrix2.Z;
            });
            break;
        case SpriteSortMode::FrontToBack:
            std::sort(std::begin(sprites), std::end(sprites), [](SpriteInfo const& a, SpriteInfo const& b) {
                return a.TransformMatrix2.Z < b.TransformMatrix2.Z;
            });
            break;
        case SpriteSortMode::Deferred:
            break;
        }

        DrawInstance(textures[queuePos], sprites);
        sprites.clear();
    }

    textures.clear();
    spriteQueues.clear();
}
//-----------------------------------------------------------------------
void SpriteRenderer::Impl::DrawInstance(std::shared_ptr<Texture2D> const& texture, std::vector<SpriteInfo> const& sprites)
{
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(sprites.size() <= MaxBatchSize);

    {
        POMDOG_ASSERT(texture->Width() > 0);
        POMDOG_ASSERT(texture->Height() > 0);

        alignas(16) Vector2 inverseTexturePixelWidth {
            (texture->Width() > 0) ? (1.0f / static_cast<float>(texture->Width())): 0.0f,
            (texture->Height() > 0) ? (1.0f / static_cast<float>(texture->Height())): 0.0f,
        };

        auto parameter = constantBuffers->Find("TextureConstants");
        parameter->SetValue(inverseTexturePixelWidth);
    }

    POMDOG_ASSERT(sprites.size() <= MaxBatchSize);
    instanceVertices->SetData(sprites.data(), sprites.size());

    graphicsContext->SetTexture(0, texture);
    graphicsContext->SetVertexBuffers({planeVertices, instanceVertices});
    graphicsContext->SetPipelineState(pipelineState);
    graphicsContext->SetConstantBuffers(constantBuffers);
    graphicsContext->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
    graphicsContext->DrawIndexedInstanced(
        planeIndices, planeIndices->IndexCount(), sprites.size());
}
//-----------------------------------------------------------------------
void SpriteRenderer::Impl::Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
    Vector2 const& position, Color const& color,
    Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth)
{
    if (scale.X == 0.0f || scale.Y == 0.0f) {
        return;
    }

    POMDOG_ASSERT(texture);

    if (spriteQueues.empty() || (textures.back() != texture)) {
        textures.push_back(texture);
        spriteQueues.push_back({});
        spriteQueues.back().reserve(MinBatchSize);
    }

    POMDOG_ASSERT(layerDepth >= 0.0f);
    POMDOG_ASSERT(layerDepth <= 1.0f);

    POMDOG_ASSERT(texture->Width() > 0);
    POMDOG_ASSERT(texture->Height() > 0);

    auto transform = Matrix3x2::CreateTranslation(position) * worldMatrix;

    SpriteInfo info;
    info.TransformMatrix1 = {transform(0, 0), transform(0, 1), transform(1, 0), transform(1, 1)};
    info.TransformMatrix2 = {transform(2, 0), transform(2, 1), layerDepth, rotation.value};
    info.SourceRect = Vector4(0, 0, texture->Width(), texture->Height());
    info.OriginScale = Vector4(originPivot.X, originPivot.Y, scale.X, scale.Y);
    info.Color = color.ToVector4();

    spriteQueues.back().push_back(std::move(info));
    POMDOG_ASSERT(spriteQueues.size() <= MaxBatchSize);
}
//-----------------------------------------------------------------------
void SpriteRenderer::Impl::Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
    Vector2 const& position, Rectangle const& sourceRect, Color const& color,
    Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth)
{
    if (scale.X == 0.0f || scale.Y == 0.0f) {
        return;
    }

    POMDOG_ASSERT(texture);

    if (spriteQueues.empty()
        || (textures.back() != texture)
        || (spriteQueues.back().size() >= MaxBatchSize))
    {
        textures.push_back(texture);
        spriteQueues.push_back({});
        spriteQueues.back().reserve(MinBatchSize);
    }

    POMDOG_ASSERT(layerDepth >= 0.0f);
    POMDOG_ASSERT(layerDepth <= 1.0f);

    POMDOG_ASSERT(sourceRect.Width > 0);
    POMDOG_ASSERT(sourceRect.Height > 0);

    auto transform = Matrix3x2::CreateTranslation(position) * worldMatrix;

    SpriteInfo info;
    info.TransformMatrix1 = {transform(0, 0), transform(0, 1), transform(1, 0), transform(1, 1)};
    info.TransformMatrix2 = {transform(2, 0), transform(2, 1), layerDepth, rotation.value};
    info.SourceRect = Vector4(sourceRect.X, sourceRect.Y, sourceRect.Width, sourceRect.Height);
    info.OriginScale = Vector4(originPivot.X, originPivot.Y, scale.X, scale.Y);
    info.Color = color.ToVector4();

    spriteQueues.back().push_back(std::move(info));
    POMDOG_ASSERT(spriteQueues.size() <= MaxBatchSize);
}
//-----------------------------------------------------------------------
// MARK: - SpriteRenderer
//-----------------------------------------------------------------------
SpriteRenderer::SpriteRenderer(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
    : impl(std::make_unique<Impl>(graphicsContext, graphicsDevice, assets))
{}
//-----------------------------------------------------------------------
SpriteRenderer::~SpriteRenderer() = default;
//-----------------------------------------------------------------------
void SpriteRenderer::SetProjectionMatrix(Matrix4x4 const& projectionMatrix)
{
    POMDOG_ASSERT(impl);
    impl->ResetProjectionMatrix(projectionMatrix);
}
//-----------------------------------------------------------------------
void SpriteRenderer::Begin(SpriteSortMode sortMode)
{
    POMDOG_ASSERT(impl);
    impl->Begin(sortMode);
}
//-----------------------------------------------------------------------
void SpriteRenderer::Begin(SpriteSortMode sortMode, Matrix4x4 const& transformMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->Begin(sortMode, transformMatrixIn);
}
//-----------------------------------------------------------------------
void SpriteRenderer::End()
{
    POMDOG_ASSERT(impl);
    impl->End();
}
//-----------------------------------------------------------------------
void SpriteRenderer::Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
    Rectangle const& sourceRect, Color const& color)
{
    POMDOG_ASSERT(impl);
    impl->Draw(texture, worldMatrix, {0, 0}, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, 0.0f);
}
//-----------------------------------------------------------------------
void SpriteRenderer::Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
    Vector2 const& position, Color const& color)
{
    POMDOG_ASSERT(impl);
    impl->Draw(texture, worldMatrix, position, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, 0.0f);
}
//-----------------------------------------------------------------------
void SpriteRenderer::Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
    Vector2 const& position, Rectangle const& sourceRect, Color const& color)
{
    POMDOG_ASSERT(impl);
    impl->Draw(texture, worldMatrix, position, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, 0.0f);
}
//-----------------------------------------------------------------------
void SpriteRenderer::Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
    Vector2 const& position, Rectangle const& sourceRect, Color const& color,
    Radian<float> const& rotation, Vector2 const& originPivot, float scale, float layerDepth)
{
    POMDOG_ASSERT(impl);
    impl->Draw(texture, worldMatrix, position, sourceRect, color, rotation, originPivot, {scale, scale}, layerDepth);
}
//-----------------------------------------------------------------------
void SpriteRenderer::Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
    Vector2 const& position, Rectangle const& sourceRect, Color const& color,
    Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth)
{
    POMDOG_ASSERT(impl);
    impl->Draw(texture, worldMatrix, position, sourceRect, color, rotation, originPivot, scale, layerDepth);
}
//-----------------------------------------------------------------------
} // namespace Pomdog
