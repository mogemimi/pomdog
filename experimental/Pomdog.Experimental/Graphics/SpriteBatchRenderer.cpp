// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "SpriteBatchRenderer.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/ConstantBufferBinding.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/IndexElementSize.hpp"
#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include <tuple>

namespace Pomdog {
namespace Detail {
namespace Rendering {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/SpriteBatchRenderer_VS.inc.hpp"
#include "Shaders/GLSL.Embedded/SpriteBatchRenderer_PS.inc.hpp"

}// unnamed namespace
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - SpriteBatchRenderer::Impl
#endif
//-----------------------------------------------------------------------
class SpriteBatchRenderer::Impl {
private:
    static constexpr std::size_t MaxBatchSize = 2048;
    static constexpr std::size_t MinBatchSize = 128;
    static constexpr std::size_t MaxTextureCount = 4;

    static_assert(MaxBatchSize >= MinBatchSize, "");

    struct alignas(16) SpriteInfo {
        // {x___} = transformMatrix.M00
        // {_y__} = transformMatrix.M01
        // {__z_} = transformMatrix.M10
        // {___w} = transformMatrix.M11
        Vector4 TransformMatrix1;

        // {x___} = transformMatrix.M20
        // {_y__} = transformMatrix.M21
        // {__zw} = originPivot.xy
        Vector4 TransformMatrix2Origin;

        // {xy__} = xy
        // {__zw} = {width, height}
        Vector4 SourceRect;

        // {rgb_} = color.rgb
        // {___a} = color.a
        Vector4 Color;

        // {x___} = textureIndex
        // {_yzw} = unused
        Vector4 TextureIndex;
    };

    struct alignas(16) TextureConstant {
        // {xy__} = InverseTextureWidths[0].xy
        // {__zw} = InverseTextureWidths[1].xy
        // {xy__} = InverseTextureWidths[2].xy
        // {__zw} = InverseTextureWidths[3].xy
        std::array<Vector4, 2> InverseTextureWidths;
    };

private:
    std::shared_ptr<GraphicsContext> graphicsContext;

    std::vector<SpriteInfo> spriteQueue;
    std::vector<std::shared_ptr<Texture2D>> textures;

    std::shared_ptr<VertexBuffer> planeVertices;
    std::shared_ptr<IndexBuffer> planeIndices;
    std::shared_ptr<VertexBuffer> instanceVertices;

    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<ConstantBufferBinding> constantBuffers;

    Matrix4x4 projectionMatrix;
    TextureConstant textureConstant;

public:
    std::uint32_t drawCallCount;

public:
    Impl(std::shared_ptr<GraphicsContext> const& graphicsContext,
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets);

    void ResetProjectionMatrix(Matrix4x4 const& projectionMatrix);

    void Begin(Matrix4x4 const& transformMatrix);

    void Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
        Color const& color, Vector2 const& originPivot);

    void Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
        Rectangle const& sourceRect, Color const& color, Vector2 const& originPivot);

    void End();

private:
    void Flush();
    void DrawInstance(std::vector<SpriteInfo> const& sprites);
    std::size_t CheckTextureIndex(std::shared_ptr<Texture2D> const& texture);
};
//-----------------------------------------------------------------------
SpriteBatchRenderer::Impl::Impl(std::shared_ptr<GraphicsContext> const& graphicsContextIn,
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
    : graphicsContext(graphicsContextIn)
    , drawCallCount(0)
{
    {
        auto viewport = graphicsContext->GetViewport();
        POMDOG_ASSERT(viewport.Width() > 0);
        POMDOG_ASSERT(viewport.Height() > 0);
        projectionMatrix = Matrix4x4::CreateOrthographicLH(viewport.Width(), viewport.Height(), 0.1f, 100.0f);
    }
    {
        using PositionTextureCoord = std::tuple<Vector4>;

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
            .SetGLSL(Builtin_GLSL_SpriteBatchRenderer_VS, std::strlen(Builtin_GLSL_SpriteBatchRenderer_VS));

        auto pixelShader = assets.CreateBuilder<Shader>()
            .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::PixelShader)
            .SetGLSL(Builtin_GLSL_SpriteBatchRenderer_PS, std::strlen(Builtin_GLSL_SpriteBatchRenderer_PS));

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
    {
        spriteQueue.reserve(MinBatchSize);
        textures.reserve(MaxTextureCount);
    }
}
//-----------------------------------------------------------------------
void SpriteBatchRenderer::Impl::ResetProjectionMatrix(Matrix4x4 const& projectionMatrixIn)
{
    this->projectionMatrix = projectionMatrixIn;
}
//-----------------------------------------------------------------------
void SpriteBatchRenderer::Impl::Begin(Matrix4x4 const& transformMatrix)
{
    alignas(16) Matrix4x4 projection = Matrix4x4::Transpose(
        transformMatrix * projectionMatrix);

    auto parameter = constantBuffers->Find("Matrices");
    parameter->SetValue(projection);

    drawCallCount = 0;
}
//-----------------------------------------------------------------------
void SpriteBatchRenderer::Impl::End()
{
    Flush();

    for (std::uint32_t index = 0; index < textures.size(); ++index) {
        graphicsContext->SetTexture(index);
    }
}
//-----------------------------------------------------------------------
void SpriteBatchRenderer::Impl::Flush()
{
    if (spriteQueue.empty()) {
        return;
    }

    POMDOG_ASSERT(!textures.empty());

    POMDOG_ASSERT(!spriteQueue.empty());
    POMDOG_ASSERT(spriteQueue.size() <= MaxBatchSize);

    DrawInstance(spriteQueue);

    textures.clear();
    spriteQueue.clear();
}
//-----------------------------------------------------------------------
void SpriteBatchRenderer::Impl::DrawInstance(std::vector<SpriteInfo> const& sprites)
{
    POMDOG_ASSERT(!textures.empty());
    POMDOG_ASSERT(textures.size() <= MaxTextureCount);
    POMDOG_ASSERT(textures.size() <= std::numeric_limits<int>::max());
    POMDOG_ASSERT(sprites.size() <= MaxBatchSize);

    auto parameter = constantBuffers->Find("TextureConstants");
    parameter->SetValue(textureConstant);

    POMDOG_ASSERT(sprites.size() <= MaxBatchSize);
    instanceVertices->SetData(sprites.data(), sprites.size());

    for (int index = 0; index < static_cast<int>(textures.size()); ++index) {
        graphicsContext->SetTexture(index, textures[index]);
    }

    for (int index = static_cast<int>(textures.size()); index < static_cast<int>(MaxTextureCount); ++index) {
        ///@note Set the dummy texture to texture unit:
        graphicsContext->SetTexture(index, textures.front());
    }

    graphicsContext->SetVertexBuffers({planeVertices, instanceVertices});
    graphicsContext->SetPipelineState(pipelineState);
    graphicsContext->SetConstantBuffers(constantBuffers);
    graphicsContext->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
    graphicsContext->DrawIndexedInstanced(
        planeIndices, planeIndices->IndexCount(), sprites.size());

    ++drawCallCount;
}
//-----------------------------------------------------------------------
std::size_t SpriteBatchRenderer::Impl::CheckTextureIndex(std::shared_ptr<Texture2D> const& texture)
{
    std::size_t textureIndex = 0;

    auto textureIter = std::find(std::begin(textures), std::end(textures), texture);
    if (textureIter == std::end(textures))
    {
        if (textures.size() >= MaxTextureCount)
        {
            Flush();
            POMDOG_ASSERT(spriteQueue.empty());
            POMDOG_ASSERT(textures.empty());
        }

        POMDOG_ASSERT(texture->Width() > 0);
        POMDOG_ASSERT(texture->Height() > 0);
        textures.push_back(texture);
        {
            POMDOG_ASSERT(texture->Width() > 0);
            POMDOG_ASSERT(texture->Height() > 0);

            Vector2 inverseTextureWidth {
                (texture->Width() > 0) ? (1.0f / static_cast<float>(texture->Width())): 0.0f,
                (texture->Height() > 0) ? (1.0f / static_cast<float>(texture->Height())): 0.0f
            };

            POMDOG_ASSERT(!textures.empty());
            textureIndex = textures.size() - 1;

            if (textureIndex % 2 == 0)
            {
                textureConstant.InverseTextureWidths[textureIndex/2].X = inverseTextureWidth.X;
                textureConstant.InverseTextureWidths[textureIndex/2].Y = inverseTextureWidth.Y;
            }
            else
            {
                textureConstant.InverseTextureWidths[textureIndex/2].Z = inverseTextureWidth.X;
                textureConstant.InverseTextureWidths[textureIndex/2].W = inverseTextureWidth.Y;
            }
        }
    }
    else
    {
        POMDOG_ASSERT(textureIter != std::end(textures));
        textureIndex = std::distance(std::begin(textures), textureIter);
    }

    return textureIndex;
}
//-----------------------------------------------------------------------
void SpriteBatchRenderer::Impl::Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& transform,
    Color const& color, Vector2 const& originPivot)
{
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(texture->Width() > 0);
    POMDOG_ASSERT(texture->Height() > 0);

    if (spriteQueue.size() >= MaxBatchSize)
    {
        Flush();
        POMDOG_ASSERT(spriteQueue.empty());
        POMDOG_ASSERT(textures.empty());
    }

    auto textureIndex = CheckTextureIndex(texture);

    SpriteInfo info;
    info.TransformMatrix1 = {transform(0, 0), transform(0, 1), transform(1, 0), transform(1, 1)};
    info.TransformMatrix2Origin = {transform(2, 0), transform(2, 1), originPivot.X, originPivot.Y};
    info.SourceRect = Vector4(0, 0, texture->Width(), texture->Height());
    info.Color = color.ToVector4();
    info.TextureIndex.X = textureIndex;

    spriteQueue.push_back(std::move(info));
    POMDOG_ASSERT(spriteQueue.size() <= MaxBatchSize);
}
//-----------------------------------------------------------------------
void SpriteBatchRenderer::Impl::Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& transform,
    Rectangle const& sourceRect, Color const& color, Vector2 const& originPivot)
{
    if (sourceRect.Width <= 0 || sourceRect.Height <= 0) {
        return;
    }

    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(texture->Width() > 0);
    POMDOG_ASSERT(texture->Height() > 0);

    if (spriteQueue.size() >= MaxBatchSize)
    {
        Flush();
        POMDOG_ASSERT(spriteQueue.empty());
        POMDOG_ASSERT(textures.empty());
    }

    auto textureIndex = CheckTextureIndex(texture);

    SpriteInfo info;
    info.TransformMatrix1 = {transform(0, 0), transform(0, 1), transform(1, 0), transform(1, 1)};
    info.TransformMatrix2Origin = {transform(2, 0), transform(2, 1), originPivot.X, originPivot.Y};
    info.SourceRect = Vector4(sourceRect.X, sourceRect.Y, sourceRect.Width, sourceRect.Height);
    info.Color = color.ToVector4();
    info.TextureIndex.X = textureIndex;

    spriteQueue.push_back(std::move(info));
    POMDOG_ASSERT(spriteQueue.size() <= MaxBatchSize);
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - SpriteBatchRenderer
#endif
//-----------------------------------------------------------------------
SpriteBatchRenderer::SpriteBatchRenderer(std::shared_ptr<GraphicsContext> const& graphicsContext,
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
    : impl(std::make_unique<Impl>(graphicsContext, graphicsDevice, assets))
{}
//-----------------------------------------------------------------------
SpriteBatchRenderer::~SpriteBatchRenderer() = default;
//-----------------------------------------------------------------------
void SpriteBatchRenderer::SetProjectionMatrix(Matrix4x4 const& projectionMatrix)
{
    POMDOG_ASSERT(impl);
    impl->ResetProjectionMatrix(projectionMatrix);
}
//-----------------------------------------------------------------------
void SpriteBatchRenderer::Begin(Matrix4x4 const& transformMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->Begin(transformMatrixIn);
}
//-----------------------------------------------------------------------
void SpriteBatchRenderer::End()
{
    POMDOG_ASSERT(impl);
    impl->End();
}
//-----------------------------------------------------------------------
void SpriteBatchRenderer::Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
    Color const& color, Vector2 const& originPivot)
{
    POMDOG_ASSERT(impl);
    impl->Draw(texture, worldMatrix, color, originPivot);
}
//-----------------------------------------------------------------------
void SpriteBatchRenderer::Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
    Rectangle const& sourceRect, Color const& color, Vector2 const& originPivot)
{
    POMDOG_ASSERT(impl);
    impl->Draw(texture, worldMatrix, sourceRect, color, originPivot);
}
//-----------------------------------------------------------------------
std::uint32_t SpriteBatchRenderer::DrawCallCount() const
{
    POMDOG_ASSERT(impl);
    return impl->drawCallCount;
}
//-----------------------------------------------------------------------
}// namespace Rendering
}// namespace Detail
}// namespace Pomdog
