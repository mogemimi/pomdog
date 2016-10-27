// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "SpriteBatchRenderer.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/IndexElementSize.hpp"
#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Graphics/VertexBufferBinding.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include <tuple>

namespace Pomdog {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/SpriteBatchRenderer_VS.inc.hpp"
#include "Shaders/GLSL.Embedded/SpriteBatchRenderer_PS.inc.hpp"
#include "Shaders/HLSL.Embedded/SpriteBatchRenderer_VS.inc.hpp"
#include "Shaders/HLSL.Embedded/SpriteBatchRenderer_PS.inc.hpp"

} // unnamed namespace

// MARK: - SpriteBatchRenderer::Impl

class SpriteBatchRenderer::Impl {
private:
    static constexpr std::size_t MaxBatchSize = 2048;
    static constexpr std::size_t MinBatchSize = 128;
    static constexpr std::size_t MaxTextureCount = 4;
    static constexpr std::size_t MaxDrawCallCount = 16;

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
    std::shared_ptr<GraphicsCommandList> commandList;

    std::vector<SpriteInfo> spriteQueue;
    std::vector<std::shared_ptr<Texture2D>> textures;

    std::shared_ptr<VertexBuffer> planeVertices;
    std::shared_ptr<IndexBuffer> planeIndices;
    std::shared_ptr<VertexBuffer> instanceVertices;

    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<ConstantBuffer> constantBufferMatrices;
    std::vector<std::shared_ptr<ConstantBuffer>> constantBuffers;

    TextureConstant textureConstant;
    std::size_t currentVertexOffset;

public:
    int drawCallCount;

public:
    Impl(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const BlendDescription& blendDescription,
        AssetManager & assets);

    void Begin(
        const std::shared_ptr<GraphicsCommandList>& commandListIn,
        const Matrix4x4& transformMatrix);

    void Draw(
        const std::shared_ptr<Texture2D>& texture,
        const Matrix3x2& worldMatrix,
        const Color& color,
        const Vector2& originPivot);

    void Draw(
        const std::shared_ptr<Texture2D>& texture,
        const Matrix3x2& worldMatrix,
        const Rectangle& sourceRect,
        const Color& color,
        const Vector2& originPivot);

    void End();

private:
    void FlushBatch();
    void RenderBatch(const std::vector<SpriteInfo>& sprites);
    std::size_t CheckTextureIndex(const std::shared_ptr<Texture2D>& texture);
};

SpriteBatchRenderer::Impl::Impl(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const BlendDescription& blendDescription,
    AssetManager & assets)
    : drawCallCount(0)
    , currentVertexOffset(0)
{
    {
        using PositionTextureCoord = Vector4;

        std::array<PositionTextureCoord, 4> const verticesCombo = {
            Vector4{0.0f, 0.0f, 0.0f, 1.0f},
            Vector4{0.0f, 1.0f, 0.0f, 0.0f},
            Vector4{1.0f, 1.0f, 1.0f, 0.0f},
            Vector4{1.0f, 0.0f, 1.0f, 1.0f},
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
        constantBufferMatrices = std::make_shared<ConstantBuffer>(
            graphicsDevice, sizeof(Matrix4x4), BufferUsage::Dynamic);

        for (std::size_t i = 0; i < MaxDrawCallCount; ++i) {
            auto textureConstants = std::make_shared<ConstantBuffer>(
                graphicsDevice, sizeof(TextureConstant), BufferUsage::Dynamic);
            constantBuffers.push_back(std::move(textureConstants));
        }
    }
    {
        auto inputLayout = InputLayoutHelper{}
            .AddInputSlot()
            .Float4()
            .AddInputSlot(InputClassification::InputPerInstance, 1)
            .Float4().Float4().Float4().Float4().Float4();

        auto vertexShader = assets.CreateBuilder<Shader>(ShaderPipelineStage::VertexShader)
            .SetGLSL(Builtin_GLSL_SpriteBatchRenderer_VS, std::strlen(Builtin_GLSL_SpriteBatchRenderer_VS))
            .SetHLSLPrecompiled(BuiltinHLSL_SpriteBatchRenderer_VS, sizeof(BuiltinHLSL_SpriteBatchRenderer_VS));

        auto pixelShader = assets.CreateBuilder<Shader>(ShaderPipelineStage::PixelShader)
            .SetGLSL(Builtin_GLSL_SpriteBatchRenderer_PS, std::strlen(Builtin_GLSL_SpriteBatchRenderer_PS))
            .SetHLSLPrecompiled(BuiltinHLSL_SpriteBatchRenderer_PS, sizeof(BuiltinHLSL_SpriteBatchRenderer_PS));

        pipelineState = assets.CreateBuilder<PipelineState>()
            .SetVertexShader(vertexShader.Build())
            .SetPixelShader(pixelShader.Build())
            .SetInputLayout(inputLayout.CreateInputLayout())
            .SetBlendState(blendDescription)
            .SetDepthStencilState(DepthStencilDescription::CreateNone())
            .SetConstantBufferBindSlot("Matrices", 0)
            .SetConstantBufferBindSlot("TextureConstants", 1)
            .Build();
    }
    {
        spriteQueue.reserve(MinBatchSize);
        textures.reserve(MaxTextureCount);
    }
}

void SpriteBatchRenderer::Impl::Begin(
    const std::shared_ptr<GraphicsCommandList>& commandListIn,
    const Matrix4x4& transformMatrix)
{
    POMDOG_ASSERT(commandListIn);
    this->commandList = commandListIn;

    alignas(16) Matrix4x4 transposed = Matrix4x4::Transpose(transformMatrix);
    constantBufferMatrices->SetValue(transposed);

    drawCallCount = 0;
    currentVertexOffset = 0;
}

void SpriteBatchRenderer::Impl::End()
{
    FlushBatch();

    if (drawCallCount > 0) {
        for (int index = 0; index < static_cast<int>(textures.size()); ++index) {
            commandList->SetTexture(index);
        }
    }
    commandList.reset();
}

void SpriteBatchRenderer::Impl::FlushBatch()
{
    if (spriteQueue.empty()) {
        return;
    }

    POMDOG_ASSERT(!textures.empty());

    POMDOG_ASSERT(!spriteQueue.empty());
    POMDOG_ASSERT(spriteQueue.size() <= MaxBatchSize);

    RenderBatch(spriteQueue);

    textures.clear();
    spriteQueue.clear();
}

void SpriteBatchRenderer::Impl::RenderBatch(const std::vector<SpriteInfo>& sprites)
{
    POMDOG_ASSERT(!textures.empty());
    POMDOG_ASSERT(textures.size() <= MaxTextureCount);
    POMDOG_ASSERT(sprites.size() <= MaxBatchSize);
    POMDOG_ASSERT(commandList);

    POMDOG_ASSERT(drawCallCount >= 0);
    POMDOG_ASSERT(drawCallCount < static_cast<int>(constantBuffers.size()));
    if (drawCallCount >= static_cast<int>(constantBuffers.size())) {
        // FUS RO DAH
        ///@todo throw exception
        return;
    }

    POMDOG_ASSERT(drawCallCount < static_cast<int>(constantBuffers.size()));
    auto constantBuffer = constantBuffers[drawCallCount];

    POMDOG_ASSERT(constantBuffer);
    constantBuffer->SetValue(textureConstant);

    POMDOG_ASSERT(sprites.size() <= MaxBatchSize);
    auto vertexOffset = sizeof(SpriteInfo) * currentVertexOffset;
    instanceVertices->SetData(
        vertexOffset,
        sprites.data(),
        sprites.size(),
        sizeof(SpriteInfo));

    for (int index = 0; index < static_cast<int>(textures.size()); ++index) {
        commandList->SetTexture(index, textures[index]);
    }

    for (int index = static_cast<int>(textures.size()); index < static_cast<int>(MaxTextureCount); ++index) {
        ///@note Set the dummy texture to texture unit:
        commandList->SetTexture(index, textures.front());
    }

    commandList->SetVertexBuffers({
        VertexBufferBinding{planeVertices},
        VertexBufferBinding{instanceVertices, vertexOffset}});
    commandList->SetPipelineState(pipelineState);
    commandList->SetConstantBuffer(0, constantBufferMatrices);
    commandList->SetConstantBuffer(1, constantBuffer);
    commandList->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
    commandList->DrawIndexedInstanced(
        planeIndices,
        planeIndices->GetIndexCount(),
        sprites.size(),
        0,
        currentVertexOffset);

    currentVertexOffset += sprites.size();
    ++drawCallCount;
}

std::size_t SpriteBatchRenderer::Impl::CheckTextureIndex(const std::shared_ptr<Texture2D>& texture)
{
    std::size_t textureIndex = 0;

    auto textureIter = std::find(std::begin(textures), std::end(textures), texture);
    if (textureIter == std::end(textures))
    {
        if (textures.size() >= MaxTextureCount) {
            FlushBatch();
            POMDOG_ASSERT(spriteQueue.empty());
            POMDOG_ASSERT(textures.empty());
        }

        POMDOG_ASSERT(texture->GetWidth() > 0);
        POMDOG_ASSERT(texture->GetHeight() > 0);
        textures.push_back(texture);
        {
            POMDOG_ASSERT(texture->GetWidth() > 0);
            POMDOG_ASSERT(texture->GetHeight() > 0);

            Vector2 inverseTextureWidth {
                (texture->GetWidth() > 0) ? (1.0f / static_cast<float>(texture->GetWidth())): 0.0f,
                (texture->GetHeight() > 0) ? (1.0f / static_cast<float>(texture->GetHeight())): 0.0f
            };

            POMDOG_ASSERT(!textures.empty());
            textureIndex = textures.size() - 1;

            if (textureIndex % 2 == 0) {
                textureConstant.InverseTextureWidths[textureIndex/2].X = inverseTextureWidth.X;
                textureConstant.InverseTextureWidths[textureIndex/2].Y = inverseTextureWidth.Y;
            }
            else {
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

void SpriteBatchRenderer::Impl::Draw(
    const std::shared_ptr<Texture2D>& texture,
    const Matrix3x2& transform,
    const Color& color,
    const Vector2& originPivot)
{
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(texture->GetWidth() > 0);
    POMDOG_ASSERT(texture->GetHeight() > 0);

    if (spriteQueue.size() >= MaxBatchSize) {
        return;
        //Flush();
        //POMDOG_ASSERT(spriteQueue.empty());
        //POMDOG_ASSERT(textures.empty());
    }

    auto textureIndex = CheckTextureIndex(texture);

    SpriteInfo info;
    info.TransformMatrix1 = {
        transform(0, 0), transform(0, 1), transform(1, 0), transform(1, 1)};
    info.TransformMatrix2Origin = {
        transform(2, 0), transform(2, 1), originPivot.X, originPivot.Y};
    info.SourceRect = Vector4(0, 0, texture->GetWidth(), texture->GetHeight());
    info.Color = color.ToVector4();
    info.TextureIndex.X = textureIndex;

    spriteQueue.push_back(std::move(info));
    POMDOG_ASSERT(spriteQueue.size() <= MaxBatchSize);
}

void SpriteBatchRenderer::Impl::Draw(
    const std::shared_ptr<Texture2D>& texture,
    const Matrix3x2& transform,
    const Rectangle& sourceRect,
    const Color& color,
    const Vector2& originPivot)
{
    if (sourceRect.Width <= 0 || sourceRect.Height <= 0) {
        return;
    }

    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(texture->GetWidth() > 0);
    POMDOG_ASSERT(texture->GetHeight() > 0);

    if (spriteQueue.size() >= MaxBatchSize) {
        return;
        //Flush();
        //POMDOG_ASSERT(spriteQueue.empty());
        //POMDOG_ASSERT(textures.empty());
    }

    auto textureIndex = CheckTextureIndex(texture);

    SpriteInfo info;
    info.TransformMatrix1 = {
        transform(0, 0), transform(0, 1), transform(1, 0), transform(1, 1)};
    info.TransformMatrix2Origin = {
        transform(2, 0), transform(2, 1), originPivot.X, originPivot.Y};
    info.SourceRect = Vector4(
        sourceRect.X, sourceRect.Y, sourceRect.Width, sourceRect.Height);
    info.Color = color.ToVector4();
    info.TextureIndex.X = textureIndex;

    spriteQueue.push_back(std::move(info));
    POMDOG_ASSERT(spriteQueue.size() <= MaxBatchSize);
}

// MARK: - SpriteBatchRenderer

SpriteBatchRenderer::SpriteBatchRenderer(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager & assets)
    : SpriteBatchRenderer(
        graphicsDevice,
        BlendDescription::CreateNonPremultiplied(),
        assets)
{}

SpriteBatchRenderer::SpriteBatchRenderer(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const BlendDescription& blendDescription,
    AssetManager & assets)
    : impl(std::make_unique<Impl>(graphicsDevice, blendDescription, assets))
{}

SpriteBatchRenderer::~SpriteBatchRenderer() = default;

void SpriteBatchRenderer::Begin(
    const std::shared_ptr<GraphicsCommandList>& commandList,
    const Matrix4x4& transformMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->Begin(commandList, transformMatrixIn);
}

void SpriteBatchRenderer::End()
{
    POMDOG_ASSERT(impl);
    impl->End();
}

void SpriteBatchRenderer::Draw(
    const std::shared_ptr<Texture2D>& texture,
    const Matrix3x2& worldMatrix,
    const Color& color,
    const Vector2& originPivot)
{
    POMDOG_ASSERT(impl);
    impl->Draw(texture, worldMatrix, color, originPivot);
}

void SpriteBatchRenderer::Draw(
    const std::shared_ptr<Texture2D>& texture,
    const Matrix3x2& worldMatrix,
    const Rectangle& sourceRect,
    const Color& color,
    const Vector2& originPivot)
{
    POMDOG_ASSERT(impl);
    impl->Draw(texture, worldMatrix, sourceRect, color, originPivot);
}

void SpriteBatchRenderer::Draw(
    const std::shared_ptr<Texture2D>& texture,
    const Rectangle& sourceRect,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->Draw(
        texture,
        Matrix3x2::Identity,
        sourceRect,
        color,
        Vector2{0.5f, 0.5f});
}

void SpriteBatchRenderer::Draw(
    const std::shared_ptr<Texture2D>& texture,
    const Vector2& position,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->Draw(
        texture,
        Matrix3x2::CreateTranslation(position),
        color,
        Vector2{0.5f, 0.5f});
}

void SpriteBatchRenderer::Draw(
    const std::shared_ptr<Texture2D>& texture,
    const Vector2& position,
    const Rectangle& sourceRect,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    impl->Draw(
        texture,
        Matrix3x2::CreateTranslation(position),
        sourceRect,
        color,
        Vector2{0.5f, 0.5f});
}

void SpriteBatchRenderer::Draw(
    const std::shared_ptr<Texture2D>& texture,
    const Vector2& position,
    const Rectangle& sourceRect,
    const Color& color,
    const Radian<float>& rotation,
    const Vector2& originPivot,
    float scale)
{
    POMDOG_ASSERT(impl);
    impl->Draw(
        texture,
        Matrix3x2::CreateScale(scale)
            * Matrix3x2::CreateRotation(rotation)
            * Matrix3x2::CreateTranslation(position),
        sourceRect, color, originPivot);
}

void SpriteBatchRenderer::Draw(
    const std::shared_ptr<Texture2D>& texture,
    const Vector2& position,
    const Rectangle& sourceRect,
    const Color& color,
    const Radian<float>& rotation,
    const Vector2& originPivot,
    const Vector2& scale)
{
    POMDOG_ASSERT(impl);
    impl->Draw(
        texture,
        Matrix3x2::CreateScale(scale)
        * Matrix3x2::CreateRotation(rotation)
        * Matrix3x2::CreateTranslation(position),
        sourceRect, color, originPivot);
}

int SpriteBatchRenderer::GetDrawCallCount() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->drawCallCount;
}

} // namespace Pomdog
