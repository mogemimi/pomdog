// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "SpriteBatchRenderer.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
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
#include "Pomdog/Graphics/SamplerState.hpp"
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
#include <cstring>
#include <tuple>

namespace Pomdog {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/SpriteBatchRenderer_VS.inc.hpp"
#include "Shaders/GLSL.Embedded/SpriteBatchRenderer_PS.inc.hpp"
#include "Shaders/HLSL.Embedded/SpriteBatchRenderer_VS.inc.hpp"
#include "Shaders/HLSL.Embedded/SpriteBatchRenderer_PS.inc.hpp"

SpriteBatchPipelineStateDescription CreateDefaultPipelineState() noexcept
{
    SpriteBatchPipelineStateDescription desc;
    desc.BlendState = BlendDescription::CreateNonPremultiplied();
    desc.SamplerState = SamplerDescription::CreateLinearWrap();
    desc.RenderTargetViewFormats.push_back(SurfaceFormat::R8G8B8A8_UNorm);
    desc.DepthStencilViewFormat = DepthFormat::Depth24Stencil8;
    return desc;
}

} // unnamed namespace

// MARK: - SpriteBatchRenderer::Impl

class SpriteBatchRenderer::Impl {
private:
    static constexpr std::size_t MaxBatchSize = 2048;
    static constexpr std::size_t MinBatchSize = 128;
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

        // {xy__} = {textureSize.width, textureSize.height}
        // {__zw} = unused
        Vector4 TextureSize;
    };

private:
    std::shared_ptr<GraphicsCommandList> commandList;

    std::vector<SpriteInfo> spriteQueue;
    std::shared_ptr<Texture2D> currentTexture;

    std::shared_ptr<VertexBuffer> planeVertices;
    std::shared_ptr<IndexBuffer> planeIndices;
    std::shared_ptr<VertexBuffer> instanceVertices;

    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<ConstantBuffer> constantBuffer;
    std::shared_ptr<SamplerState> sampler;

    std::size_t startInstanceLocation;

    Vector2 inverseTextureSize;

public:
    int drawCallCount;

public:
    Impl(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const SpriteBatchPipelineStateDescription& pipelineStateDescription,
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

    void FlushBatch();

private:
    void RenderBatch(
        const std::shared_ptr<Texture2D>& texture,
        const std::vector<SpriteInfo>& sprites);

    void CompareTexture(const std::shared_ptr<Texture2D>& texture);
};

SpriteBatchRenderer::Impl::Impl(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const SpriteBatchPipelineStateDescription& pipelineStateDescription,
    AssetManager & assets)
    : startInstanceLocation(0)
    , drawCallCount(0)
{
    {
        using PositionTextureCoord = Vector4;

        std::array<PositionTextureCoord, 4> const verticesCombo = {{
            Vector4{0.0f, 0.0f, 0.0f, 1.0f},
            Vector4{0.0f, 1.0f, 0.0f, 0.0f},
            Vector4{1.0f, 1.0f, 1.0f, 0.0f},
            Vector4{1.0f, 0.0f, 1.0f, 1.0f},
        }};

        planeVertices = std::make_shared<VertexBuffer>(
            graphicsDevice,
            verticesCombo.data(),
            verticesCombo.size(),
            sizeof(PositionTextureCoord),
            BufferUsage::Immutable);
    }
    {
        std::array<std::uint16_t, 6> const indices = {{
            0, 1, 2,
            2, 3, 0
        }};

        // Create index buffer
        planeIndices = std::make_shared<IndexBuffer>(
            graphicsDevice,
            IndexElementSize::SixteenBits,
            indices.data(),
            indices.size(),
            BufferUsage::Immutable);
    }
    {
        const auto maxBatchSize = MaxBatchSize;
        instanceVertices = std::make_shared<VertexBuffer>(
            graphicsDevice,
            maxBatchSize,
            sizeof(SpriteInfo),
            BufferUsage::Dynamic);
    }
    {
        constantBuffer = std::make_shared<ConstantBuffer>(
            graphicsDevice,
            sizeof(Matrix4x4),
            BufferUsage::Dynamic);

        sampler = std::make_shared<SamplerState>(
            graphicsDevice,
            pipelineStateDescription.SamplerState);
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
            .SetBlendState(pipelineStateDescription.BlendState)
            .SetRenderTargetViewFormats(pipelineStateDescription.RenderTargetViewFormats)
            .SetDepthStencilViewFormat(pipelineStateDescription.DepthStencilViewFormat)
            .SetDepthStencilState(DepthStencilDescription::CreateNone())
            .SetConstantBufferBindSlot("SpriteBatchConstants", 0)
            .Build();
    }
    {
        spriteQueue.reserve(MinBatchSize);
    }
}

void SpriteBatchRenderer::Impl::Begin(
    const std::shared_ptr<GraphicsCommandList>& commandListIn,
    const Matrix4x4& transformMatrix)
{
    POMDOG_ASSERT(commandListIn);
    POMDOG_ASSERT(constantBuffer);

    this->commandList = commandListIn;

    alignas(16) Matrix4x4 transposedMatrix = Matrix4x4::Transpose(transformMatrix);
    constantBuffer->SetValue(transposedMatrix);

    drawCallCount = 0;
    startInstanceLocation = 0;
}

void SpriteBatchRenderer::Impl::End()
{
    FlushBatch();

    if (drawCallCount > 0) {
        commandList->SetTexture(0);
    }
    commandList.reset();
}

void SpriteBatchRenderer::Impl::FlushBatch()
{
    if (spriteQueue.empty()) {
        return;
    }

    POMDOG_ASSERT(currentTexture != nullptr);
    POMDOG_ASSERT(!spriteQueue.empty());
    POMDOG_ASSERT((startInstanceLocation + spriteQueue.size()) <= MaxBatchSize);

    RenderBatch(currentTexture, spriteQueue);

    currentTexture = nullptr;
    spriteQueue.clear();
}

void SpriteBatchRenderer::Impl::RenderBatch(
    const std::shared_ptr<Texture2D>& texture,
    const std::vector<SpriteInfo>& sprites)
{
    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(!sprites.empty());
    POMDOG_ASSERT(drawCallCount >= 0);

    POMDOG_ASSERT((startInstanceLocation + sprites.size()) <= MaxBatchSize);
    const auto instanceOffsetBytes = sizeof(SpriteInfo) * startInstanceLocation;
    instanceVertices->SetData(
        instanceOffsetBytes,
        sprites.data(),
        sprites.size(),
        sizeof(SpriteInfo));

    commandList->SetTexture(0, texture);

    commandList->SetSamplerState(0, sampler);

    commandList->SetPipelineState(pipelineState);
    commandList->SetConstantBuffer(0, constantBuffer);
    commandList->SetPrimitiveTopology(PrimitiveTopology::TriangleList);

#define POMDOG_USE_OPENGL4_1_COMPATIBLE_MODE 1
#if POMDOG_USE_OPENGL4_1_COMPATIBLE_MODE
    commandList->SetVertexBuffers({
        VertexBufferBinding{planeVertices},
        VertexBufferBinding{instanceVertices, instanceOffsetBytes}
    });

    commandList->DrawIndexedInstanced(
        planeIndices,
        planeIndices->GetIndexCount(),
        sprites.size(),
        0,
        0);
#else
    // The following code is supported on OpenGL 4.2+, not OpenGL 4.1.
    commandList->SetVertexBuffers({
        VertexBufferBinding{planeVertices},
        VertexBufferBinding{instanceVertices}
    });

    commandList->DrawIndexedInstanced(
        planeIndices,
        planeIndices->GetIndexCount(),
        sprites.size(),
        0,
        startInstanceLocation);
#endif

    startInstanceLocation += sprites.size();
    POMDOG_ASSERT(startInstanceLocation <= MaxBatchSize);

    ++drawCallCount;
}

void SpriteBatchRenderer::Impl::CompareTexture(const std::shared_ptr<Texture2D>& texture)
{
    POMDOG_ASSERT(texture != nullptr);

    if (texture != currentTexture) {
        if (currentTexture != nullptr) {
            FlushBatch();
        }
        POMDOG_ASSERT(spriteQueue.empty());
        POMDOG_ASSERT(currentTexture == nullptr);

        currentTexture = texture;

        POMDOG_ASSERT(texture->GetWidth() > 0);
        POMDOG_ASSERT(texture->GetHeight() > 0);

        inverseTextureSize.X = (texture->GetWidth() > 0) ? (1.0f / static_cast<float>(texture->GetWidth())) : 0.0f;
        inverseTextureSize.Y = (texture->GetHeight() > 0) ? (1.0f / static_cast<float>(texture->GetHeight())) : 0.0f;
    }
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

    if ((startInstanceLocation + spriteQueue.size()) >= MaxBatchSize) {
        FlushBatch();
        POMDOG_ASSERT(spriteQueue.empty());

        // TODO: Not implemented
        //GrowSpriteQueue();
        return;
    }

    POMDOG_ASSERT((startInstanceLocation + spriteQueue.size()) < MaxBatchSize);

    CompareTexture(texture);

    SpriteInfo info;
    info.TransformMatrix1 = {
        transform(0, 0),
        transform(0, 1),
        transform(1, 0),
        transform(1, 1)
    };
    info.TransformMatrix2Origin = {
        transform(2, 0),
        transform(2, 1),
        originPivot.X,
        originPivot.Y
    };
    info.SourceRect = Vector4{
        0.0f,
        0.0f,
        static_cast<float>(texture->GetWidth()),
        static_cast<float>(texture->GetHeight())
    };
    info.Color = color.ToVector4();
    info.TextureSize = Vector4{
        inverseTextureSize.X,
        inverseTextureSize.Y,
        0.0f,
        0.0f
    };

    spriteQueue.push_back(std::move(info));
    POMDOG_ASSERT((startInstanceLocation + spriteQueue.size()) <= MaxBatchSize);
}

void SpriteBatchRenderer::Impl::Draw(
    const std::shared_ptr<Texture2D>& texture,
    const Matrix3x2& transform,
    const Rectangle& sourceRect,
    const Color& color,
    const Vector2& originPivot)
{
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(texture->GetWidth() > 0);
    POMDOG_ASSERT(texture->GetHeight() > 0);
    POMDOG_ASSERT(sourceRect.Width >= 0);
    POMDOG_ASSERT(sourceRect.Height >= 0);

    if (sourceRect.Width == 0 || sourceRect.Height == 0) {
        return;
    }

    if ((startInstanceLocation + spriteQueue.size()) >= MaxBatchSize) {
        FlushBatch();
        POMDOG_ASSERT(spriteQueue.empty());

        // TODO: Not implemented
        //GrowSpriteQueue();
        return;
    }

    POMDOG_ASSERT((startInstanceLocation + spriteQueue.size()) < MaxBatchSize);
    POMDOG_ASSERT(sourceRect.Width > 0);
    POMDOG_ASSERT(sourceRect.Height > 0);

    CompareTexture(texture);

    SpriteInfo info;
    info.TransformMatrix1 = {
        transform(0, 0),
        transform(0, 1),
        transform(1, 0),
        transform(1, 1)
    };
    info.TransformMatrix2Origin = {
        transform(2, 0),
        transform(2, 1),
        originPivot.X,
        originPivot.Y
    };
    info.SourceRect = Vector4{
        static_cast<float>(sourceRect.X),
        static_cast<float>(sourceRect.Y),
        static_cast<float>(sourceRect.Width),
        static_cast<float>(sourceRect.Height)
    };
    info.Color = color.ToVector4();
    info.TextureSize = Vector4{
        inverseTextureSize.X,
        inverseTextureSize.Y,
        0.0f,
        0.0f
    };

    spriteQueue.push_back(std::move(info));
    POMDOG_ASSERT((startInstanceLocation + spriteQueue.size()) <= MaxBatchSize);
}

// MARK: - SpriteBatchRenderer

SpriteBatchRenderer::SpriteBatchRenderer(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager & assets)
    : SpriteBatchRenderer(
        graphicsDevice,
        CreateDefaultPipelineState(),
        assets)
{}

SpriteBatchRenderer::SpriteBatchRenderer(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const SpriteBatchPipelineStateDescription& pipelineStateDescription,
    AssetManager & assets)
    : impl(std::make_unique<Impl>(graphicsDevice, pipelineStateDescription, assets))
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

void SpriteBatchRenderer::Flush()
{
    POMDOG_ASSERT(impl);
    impl->FlushBatch();
}

int SpriteBatchRenderer::GetDrawCallCount() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->drawCallCount;
}

} // namespace Pomdog
