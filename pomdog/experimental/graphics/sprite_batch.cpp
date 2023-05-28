// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/asset_builders/shader_builder.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/experimental/graphics/texture2d_view.h"
#include "pomdog/experimental/texture_packer/texture_region.h"
#include "pomdog/gpu/blend_descriptor.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/depth_stencil_descriptor.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/index_format.h"
#include "pomdog/gpu/input_layout_helper.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/rasterizer_descriptor.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/sampler_descriptor.h"
#include "pomdog/gpu/sampler_state.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_language.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/math/color.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"
#include "pomdog/memory/aligned_new.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstring>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::detail::AlignedNew;

namespace pomdog {
namespace {

// Built-in shaders
#include "shaders/glsl.embedded/sprite_batch_distance_field_ps.inc.h"
#include "shaders/glsl.embedded/sprite_batch_ps.inc.h"
#include "shaders/glsl.embedded/sprite_batch_vs.inc.h"
#include "shaders/hlsl.embedded/sprite_batch_distance_field_ps.inc.h"
#include "shaders/hlsl.embedded/sprite_batch_ps.inc.h"
#include "shaders/hlsl.embedded/sprite_batch_vs.inc.h"
#include "shaders/metal.embedded/sprite_batch.inc.h"

Vector2 ComputeSpriteOffset(const TextureRegion& region, const Vector2& originPivot) noexcept
{
    if ((region.subrect.width <= 0) || (region.subrect.height <= 0)) {
        return Vector2::createZero();
    }

    POMDOG_ASSERT(region.subrect.width > 0);
    POMDOG_ASSERT(region.subrect.height > 0);

    const auto regionSize = Vector2{
        static_cast<float>(region.width),
        static_cast<float>(region.height)};

    const auto baseOffset = regionSize * originPivot;

    const auto w = static_cast<float>(region.subrect.width);
    const auto h = static_cast<float>(region.subrect.height);

    auto offset = Vector2{
        static_cast<float>(region.xOffset),
        regionSize.y - (static_cast<float>(region.yOffset) + h)};
    offset = (baseOffset - offset) / Vector2{w, h};
    return offset;
}

struct alignas(16) SpriteBatchConstantBuffer final {
    Matrix4x4 ViewProjection;

    // {x___} = Smoothing
    // {_y__} = Weight
    // {__zw} = unused
    Vector4 DistanceFieldParameters;
};

} // namespace

// MARK: - SpriteBatch::Impl

class SpriteBatch::Impl final {
private:
    static constexpr std::size_t MaxBatchSize = 2048;
    static constexpr std::size_t MinBatchSize = 128;
    static constexpr std::size_t MaxDrawCallCount = 16;

    static_assert(MaxBatchSize >= MinBatchSize, "");

    struct alignas(16) SpriteInfo final : public AlignedNew<SpriteInfo> {
        // {xy__} = position.xy
        // {__zw} = scale.xy
        Vector4 Translation;

        // {xy__} = xy
        // {__zw} = {width, height}
        Vector4 SourceRect;

        // {xy__} = originPivot.xy
        // {__z_} = rotation
        // {___w} = layerDepth
        Vector4 OriginRotationLayerDepth;

        // {rgb_} = color.rgb
        // {___a} = color.a
        Vector4 Color;

        // {xy__} = {1.0f / textureWidth, 1.0f / textureHeight}
        // {__z_} = RGBA channel flags (8-bits)
        // {___w} = unused
        Vector4 InverseTextureSize;
    };

private:
    std::vector<SpriteInfo> spriteQueue;

    std::shared_ptr<gpu::CommandList> commandList;
    Texture2DView currentTexture;

    std::shared_ptr<gpu::VertexBuffer> planeVertices;
    std::shared_ptr<gpu::IndexBuffer> planeIndices;
    std::shared_ptr<gpu::VertexBuffer> instanceVertices;

    std::shared_ptr<gpu::PipelineState> pipelineState;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer;
    std::shared_ptr<gpu::SamplerState> sampler;

    Vector2 inverseTextureSize;
    std::size_t startInstanceLocation;

public:
    int drawCallCount;

public:
    Impl(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<gpu::BlendDescriptor>&& blendState,
        std::optional<gpu::RasterizerDescriptor>&& rasterizerDesc,
        std::optional<gpu::SamplerDescriptor>&& samplerState,
        std::optional<PixelFormat>&& renderTargetViewFormat,
        std::optional<PixelFormat>&& depthStencilViewFormat,
        SpriteBatchPixelShaderMode pixelShaderMode,
        AssetManager& assets);

    void Begin(
        const std::shared_ptr<gpu::CommandList>& commandListIn,
        const Matrix4x4& transformMatrix,
        std::optional<SpriteBatchDistanceFieldParameters>&& distanceFieldParameters);

    void Draw(
        const Texture2DView& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        const Vector2& scale,
        float layerDepth);

    void FlushBatch();

    void End();

private:
    void RenderBatch(
        const Texture2DView& texture,
        const std::vector<SpriteInfo>& sprites);

    void CompareTexture(const Texture2DView& texture);
};

SpriteBatch::Impl::Impl(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::BlendDescriptor>&& blendDesc,
    std::optional<gpu::RasterizerDescriptor>&& rasterizerDesc,
    std::optional<gpu::SamplerDescriptor>&& samplerDesc,
    std::optional<PixelFormat>&& renderTargetViewFormat,
    std::optional<PixelFormat>&& depthStencilViewFormat,
    SpriteBatchPixelShaderMode pixelShaderMode,
    AssetManager& assets)
    : startInstanceLocation(0)
    , drawCallCount(0)
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    if (!blendDesc) {
        blendDesc = gpu::BlendDescriptor::createNonPremultiplied();
    }
    if (!rasterizerDesc) {
        rasterizerDesc = gpu::RasterizerDescriptor::createCullNone();
    }
    if (!samplerDesc) {
        samplerDesc = gpu::SamplerDescriptor::createLinearWrap();
    }
    if (!renderTargetViewFormat) {
        renderTargetViewFormat = presentationParameters.backBufferFormat;
    }
    if (!depthStencilViewFormat) {
        depthStencilViewFormat = presentationParameters.depthStencilFormat;
    }

    POMDOG_ASSERT(blendDesc);
    POMDOG_ASSERT(rasterizerDesc);
    POMDOG_ASSERT(samplerDesc);
    POMDOG_ASSERT(renderTargetViewFormat);
    POMDOG_ASSERT(depthStencilViewFormat);

    {
        using PositionTextureCoord = Vector4;

        std::array<PositionTextureCoord, 4> const verticesCombo = {{
            Vector4{0.0f, 0.0f, 0.0f, 1.0f},
            Vector4{0.0f, 1.0f, 0.0f, 0.0f},
            Vector4{1.0f, 1.0f, 1.0f, 0.0f},
            Vector4{1.0f, 0.0f, 1.0f, 1.0f},
        }};
        planeVertices = std::get<0>(graphicsDevice->CreateVertexBuffer(
            verticesCombo.data(),
            verticesCombo.size(),
            sizeof(PositionTextureCoord),
            gpu::BufferUsage::Immutable));
    }
    {
        std::array<std::uint16_t, 6> const indices = {{0, 1, 2, 2, 3, 0}};

        // Create index buffer
        planeIndices = std::get<0>(graphicsDevice->CreateIndexBuffer(
            gpu::IndexFormat::UInt16,
            indices.data(),
            indices.size(),
            gpu::BufferUsage::Immutable));
    }
    {
        const auto maxBatchSize = MaxBatchSize;
        instanceVertices = std::get<0>(graphicsDevice->CreateVertexBuffer(
            maxBatchSize,
            sizeof(SpriteInfo),
            gpu::BufferUsage::Dynamic));
    }
    {
        constantBuffer = std::get<0>(graphicsDevice->CreateConstantBuffer(
            sizeof(SpriteBatchConstantBuffer),
            gpu::BufferUsage::Dynamic));

        sampler = std::get<0>(graphicsDevice->CreateSamplerState(
            *samplerDesc));
    }
    {
        auto inputLayout = gpu::InputLayoutHelper{}
                               .AddInputSlot()
                               .Float4()
                               .AddInputSlot(gpu::InputClassification::InputPerInstance, 1)
                               .Float4()
                               .Float4()
                               .Float4()
                               .Float4()
                               .Float4();

        auto vertexShaderBuilder = assets.CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader)
                                       .SetGLSL(Builtin_GLSL_SpriteBatch_VS, std::strlen(Builtin_GLSL_SpriteBatch_VS))
                                       .SetHLSLPrecompiled(BuiltinHLSL_SpriteBatch_VS, sizeof(BuiltinHLSL_SpriteBatch_VS))
                                       .SetMetal(Builtin_Metal_SpriteBatch, sizeof(Builtin_Metal_SpriteBatch), "SpriteBatchVS");

        auto [vertexShader, vertexShaderErr] = vertexShaderBuilder.Build();
        if (vertexShaderErr != nullptr) {
            // FIXME: error handling
        }

        auto pixelShaderBuilder = assets.CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader);

        switch (pixelShaderMode) {
        case SpriteBatchPixelShaderMode::Default:
            pixelShaderBuilder.SetGLSL(Builtin_GLSL_SpriteBatch_PS, std::strlen(Builtin_GLSL_SpriteBatch_PS));
            pixelShaderBuilder.SetHLSLPrecompiled(BuiltinHLSL_SpriteBatch_PS, sizeof(BuiltinHLSL_SpriteBatch_PS));
            pixelShaderBuilder.SetMetal(Builtin_Metal_SpriteBatch, sizeof(Builtin_Metal_SpriteBatch), "SpriteBatchPS");
            break;
        case SpriteBatchPixelShaderMode::DistanceField:
            pixelShaderBuilder.SetGLSL(Builtin_GLSL_SpriteBatchDistanceField_PS, std::strlen(Builtin_GLSL_SpriteBatchDistanceField_PS));
            pixelShaderBuilder.SetHLSLPrecompiled(BuiltinHLSL_SpriteBatchDistanceField_PS, sizeof(BuiltinHLSL_SpriteBatchDistanceField_PS));
            pixelShaderBuilder.SetMetal(Builtin_Metal_SpriteBatch, sizeof(Builtin_Metal_SpriteBatch), "SpriteBatchDistanceFieldPS");
            break;
        }

        auto [pixelShader, pixelShaderErr] = pixelShaderBuilder.Build();
        if (pixelShaderErr != nullptr) {
            // FIXME: error handling
        }

        std::unique_ptr<Error> pipelineStateErr;
        std::tie(pipelineState, pipelineStateErr) = assets.CreateBuilder<gpu::PipelineState>()
                                                        .SetRenderTargetViewFormat(*renderTargetViewFormat)
                                                        .SetDepthStencilViewFormat(*depthStencilViewFormat)
                                                        .SetVertexShader(std::move(vertexShader))
                                                        .SetPixelShader(std::move(pixelShader))
                                                        .SetInputLayout(inputLayout.CreateInputLayout())
                                                        .SetPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
                                                        .SetBlendState(*blendDesc)
                                                        .SetDepthStencilState(gpu::DepthStencilDescriptor::createNone())
                                                        .SetRasterizerState(*rasterizerDesc)
                                                        .SetConstantBufferBindSlot("SpriteBatchConstants", 0)
                                                        .Build();

        if (pipelineStateErr != nullptr) {
            // FIXME: error handling
        }
    }

    spriteQueue.reserve(MinBatchSize);
}

void SpriteBatch::Impl::Begin(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const Matrix4x4& transformMatrix,
    std::optional<SpriteBatchDistanceFieldParameters>&& distanceFieldParameters)
{
    POMDOG_ASSERT(commandListIn);
    this->commandList = commandListIn;

    POMDOG_ASSERT(constantBuffer);

    SpriteBatchConstantBuffer constants;
    constants.ViewProjection = math::transpose(transformMatrix);

    if (distanceFieldParameters != std::nullopt) {
        constants.DistanceFieldParameters.x = distanceFieldParameters->Smoothing;
        constants.DistanceFieldParameters.y = distanceFieldParameters->Weight;
    }
    else {
        constants.DistanceFieldParameters.x = 0.25f;
        constants.DistanceFieldParameters.y = 0.65f;
    }

    constantBuffer->setData(0, gpu::makeByteSpan(constants));

    startInstanceLocation = 0;
    drawCallCount = 0;
}

void SpriteBatch::Impl::End()
{
    FlushBatch();

    if (drawCallCount > 0) {
        commandList->SetTexture(0);
    }
    commandList.reset();
}

void SpriteBatch::Impl::FlushBatch()
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

void SpriteBatch::Impl::RenderBatch(
    const Texture2DView& texture,
    const std::vector<SpriteInfo>& sprites)
{
    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(!sprites.empty());
    POMDOG_ASSERT((startInstanceLocation + sprites.size()) <= MaxBatchSize);

    POMDOG_ASSERT(drawCallCount >= 0);

    POMDOG_ASSERT(sprites.size() <= MaxBatchSize);
    const auto instanceOffsetBytes = sizeof(SpriteInfo) * startInstanceLocation;
    instanceVertices->setData(
        instanceOffsetBytes,
        sprites.data(),
        sprites.size(),
        sizeof(SpriteInfo));

    if (texture.GetIndex() == Texture2DViewIndex::Texture2D) {
        commandList->SetTexture(0, texture.AsTexture2D());
    }
    else if (texture.GetIndex() == Texture2DViewIndex::RenderTarget2D) {
        commandList->SetTexture(0, texture.AsRenderTarget2D());
    }
    commandList->SetSamplerState(0, sampler);

    commandList->SetPipelineState(pipelineState);
    commandList->SetConstantBuffer(0, constantBuffer);
    commandList->SetVertexBuffer(0, planeVertices);
    commandList->SetVertexBuffer(1, instanceVertices);
    commandList->SetIndexBuffer(planeIndices);

    commandList->DrawIndexedInstanced(
        planeIndices->getIndexCount(),
        sprites.size(),
        0,
        startInstanceLocation);

    startInstanceLocation += sprites.size();
    POMDOG_ASSERT(startInstanceLocation <= MaxBatchSize);

    ++drawCallCount;
}

void SpriteBatch::Impl::CompareTexture(const Texture2DView& texture)
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

        const float w = static_cast<float>(texture->GetWidth());
        const float h = static_cast<float>(texture->GetHeight());

        inverseTextureSize.x = (w > 0.0f) ? (1.0f / w) : 0.0f;
        inverseTextureSize.y = (h > 0.0f) ? (1.0f / h) : 0.0f;
    }
}

void SpriteBatch::Impl::Draw(
    const Texture2DView& texture,
    const Vector2& position,
    const Rectangle& sourceRect,
    const Color& color,
    const Radian<float>& rotation,
    const Vector2& originPivot,
    const Vector2& scale,
    float layerDepth)
{
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(texture->GetWidth() > 0);
    POMDOG_ASSERT(texture->GetHeight() > 0);
    POMDOG_ASSERT(sourceRect.width >= 0);
    POMDOG_ASSERT(sourceRect.height >= 0);

    if ((sourceRect.width == 0) || (sourceRect.height == 0)) {
        return;
    }

    if ((scale.x == 0.0f) || (scale.y == 0.0f)) {
        return;
    }

    if ((startInstanceLocation + spriteQueue.size()) >= MaxBatchSize) {
        FlushBatch();
        POMDOG_ASSERT(spriteQueue.empty());

        // TODO: Not implemented
        // GrowSpriteQueue();
        return;
    }

    bool sourceRGBEnabled = true;
    bool sourceAlphaEnabled = true;
    bool compensationRGB = false;
    bool compensationAlpha = false;

    switch (texture->GetFormat()) {
    case PixelFormat::R8_UNorm:
    case PixelFormat::R8G8_UNorm:
    case PixelFormat::R16G16_Float:
    case PixelFormat::R11G11B10_Float:
    case PixelFormat::R32_Float:
        sourceAlphaEnabled = false;
        compensationAlpha = true;
        break;
    case PixelFormat::A8_UNorm:
        sourceRGBEnabled = false;
        compensationRGB = true;
        break;
    case PixelFormat::Invalid:
    case PixelFormat::R8G8B8A8_UNorm:
    case PixelFormat::R10G10B10A2_UNorm:
    case PixelFormat::B8G8R8A8_UNorm:
    case PixelFormat::R16G16B16A16_Float:
    case PixelFormat::R32G32B32A32_Float:
    case PixelFormat::BlockComp1_UNorm:
    case PixelFormat::BlockComp2_UNorm:
    case PixelFormat::BlockComp3_UNorm:
    case PixelFormat::Depth16:
    case PixelFormat::Depth32:
    case PixelFormat::Depth24Stencil8:
    case PixelFormat::Depth32_Float_Stencil8_Uint:
        break;
    }

    const int colorModeFlags =
        (sourceRGBEnabled ? 1 : 0) |
        (sourceAlphaEnabled ? 2 : 0) |
        (compensationRGB ? 4 : 0) |
        (compensationAlpha ? 8 : 0);

    POMDOG_ASSERT((startInstanceLocation + spriteQueue.size()) < MaxBatchSize);
    POMDOG_ASSERT(sourceRect.width > 0);
    POMDOG_ASSERT(sourceRect.height > 0);

    CompareTexture(texture);

    SpriteInfo info;
    info.Translation = Vector4{
        position.x,
        position.y,
        scale.x,
        scale.y,
    };
    info.SourceRect = Vector4{
        static_cast<float>(sourceRect.x),
        static_cast<float>(sourceRect.y),
        static_cast<float>(sourceRect.width),
        static_cast<float>(sourceRect.height),
    };
    info.OriginRotationLayerDepth = Vector4{
        originPivot.x,
        originPivot.y,
        rotation.value,
        layerDepth,
    };
    info.Color = color.toVector4();
    info.InverseTextureSize = Vector4{
        inverseTextureSize.x,
        inverseTextureSize.y,
        static_cast<float>(colorModeFlags),
        0.0f,
    };

    spriteQueue.push_back(std::move(info));
    POMDOG_ASSERT((startInstanceLocation + spriteQueue.size()) <= MaxBatchSize);
}

// MARK: - SpriteBatch

SpriteBatch::SpriteBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
    : SpriteBatch(
          graphicsDevice,
          std::nullopt,
          std::nullopt,
          std::nullopt,
          std::nullopt,
          std::nullopt,
          SpriteBatchPixelShaderMode::Default,
          assets)
{
}

SpriteBatch::SpriteBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::BlendDescriptor>&& blendDesc,
    std::optional<gpu::RasterizerDescriptor>&& rasterizerDesc,
    std::optional<gpu::SamplerDescriptor>&& samplerDesc,
    std::optional<PixelFormat>&& renderTargetViewFormat,
    std::optional<PixelFormat>&& depthStencilViewFormat,
    SpriteBatchPixelShaderMode pixelShaderMode,
    AssetManager& assets)
    : impl(std::make_unique<Impl>(
          graphicsDevice,
          std::move(blendDesc),
          std::move(rasterizerDesc),
          std::move(samplerDesc),
          std::move(renderTargetViewFormat),
          std::move(depthStencilViewFormat),
          pixelShaderMode,
          assets))
{
}

SpriteBatch::~SpriteBatch() = default;

void SpriteBatch::Begin(
    const std::shared_ptr<gpu::CommandList>& commandList,
    const Matrix4x4& transformMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->Begin(commandList, transformMatrixIn, std::nullopt);
}

void SpriteBatch::Begin(
    const std::shared_ptr<gpu::CommandList>& commandList,
    const Matrix4x4& transformMatrixIn,
    const SpriteBatchDistanceFieldParameters& distanceFieldParameters)
{
    POMDOG_ASSERT(impl);
    impl->Begin(commandList, transformMatrixIn, distanceFieldParameters);
}

void SpriteBatch::Flush()
{
    POMDOG_ASSERT(impl);
    impl->FlushBatch();
}

void SpriteBatch::End()
{
    POMDOG_ASSERT(impl);
    impl->End();
}

void SpriteBatch::Draw(
    const std::shared_ptr<gpu::Texture2D>& texture,
    const Rectangle& sourceRect,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    constexpr float layerDepth = 0.0f;
    impl->Draw(texture, {0, 0}, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, layerDepth);
}

void SpriteBatch::Draw(
    const std::shared_ptr<gpu::Texture2D>& texture,
    const Vector2& position,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    constexpr float layerDepth = 0.0f;
    const Rectangle sourceRect = {0, 0, texture->getWidth(), texture->getHeight()};
    impl->Draw(texture, position, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, layerDepth);
}

void SpriteBatch::Draw(
    const std::shared_ptr<gpu::Texture2D>& texture,
    const Vector2& position,
    const Rectangle& sourceRect,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    constexpr float layerDepth = 0.0f;
    impl->Draw(texture, position, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, layerDepth);
}

void SpriteBatch::Draw(
    const std::shared_ptr<gpu::Texture2D>& texture,
    const Vector2& position,
    const Rectangle& sourceRect,
    const Color& color,
    const Radian<float>& rotation,
    const Vector2& originPivot,
    float scale)
{
    POMDOG_ASSERT(impl);
    constexpr float layerDepth = 0.0f;
    impl->Draw(texture, position, sourceRect, color, rotation, originPivot, {scale, scale}, layerDepth);
}

void SpriteBatch::Draw(
    const std::shared_ptr<gpu::Texture2D>& texture,
    const Vector2& position,
    const Rectangle& sourceRect,
    const Color& color,
    const Radian<float>& rotation,
    const Vector2& originPivot,
    const Vector2& scale)
{
    POMDOG_ASSERT(impl);
    constexpr float layerDepth = 0.0f;
    impl->Draw(texture, position, sourceRect, color, rotation, originPivot, scale, layerDepth);
}

void SpriteBatch::Draw(
    const std::shared_ptr<gpu::Texture2D>& texture,
    const Vector2& position,
    const TextureRegion& textureRegion,
    const Color& color,
    const Radian<float>& rotation,
    const Vector2& originPivot,
    float scale)
{
    POMDOG_ASSERT(impl);
    auto offset = ComputeSpriteOffset(textureRegion, originPivot);
    constexpr float layerDepth = 0.0f;
    impl->Draw(texture, position, textureRegion.subrect, color, rotation, offset, {scale, scale}, layerDepth);
}

void SpriteBatch::Draw(
    const std::shared_ptr<gpu::Texture2D>& texture,
    const Vector2& position,
    const TextureRegion& textureRegion,
    const Color& color,
    const Radian<float>& rotation,
    const Vector2& originPivot,
    const Vector2& scale)
{
    POMDOG_ASSERT(impl);
    auto offset = ComputeSpriteOffset(textureRegion, originPivot);
    constexpr float layerDepth = 0.0f;
    impl->Draw(texture, position, textureRegion.subrect, color, rotation, offset, scale, layerDepth);
}

void SpriteBatch::Draw(
    const std::shared_ptr<gpu::RenderTarget2D>& texture,
    const Rectangle& sourceRect,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    constexpr float layerDepth = 0.0f;
    impl->Draw(texture, {0, 0}, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, layerDepth);
}

void SpriteBatch::Draw(
    const std::shared_ptr<gpu::RenderTarget2D>& texture,
    const Vector2& position,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    constexpr float layerDepth = 0.0f;
    const Rectangle sourceRect = {0, 0, texture->getWidth(), texture->getHeight()};
    impl->Draw(texture, position, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, layerDepth);
}

void SpriteBatch::Draw(
    const std::shared_ptr<gpu::RenderTarget2D>& texture,
    const Vector2& position,
    const Rectangle& sourceRect,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    constexpr float layerDepth = 0.0f;
    impl->Draw(texture, position, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, layerDepth);
}

void SpriteBatch::Draw(
    const std::shared_ptr<gpu::RenderTarget2D>& texture,
    const Vector2& position,
    const Rectangle& sourceRect,
    const Color& color,
    const Radian<float>& rotation,
    const Vector2& originPivot,
    float scale)
{
    POMDOG_ASSERT(impl);
    constexpr float layerDepth = 0.0f;
    impl->Draw(texture, position, sourceRect, color, rotation, originPivot, {scale, scale}, layerDepth);
}

void SpriteBatch::Draw(
    const std::shared_ptr<gpu::RenderTarget2D>& texture,
    const Vector2& position,
    const Rectangle& sourceRect,
    const Color& color,
    const Radian<float>& rotation,
    const Vector2& originPivot,
    const Vector2& scale)
{
    POMDOG_ASSERT(impl);
    constexpr float layerDepth = 0.0f;
    impl->Draw(texture, position, sourceRect, color, rotation, originPivot, scale, layerDepth);
}

void SpriteBatch::Draw(
    const std::shared_ptr<gpu::RenderTarget2D>& texture,
    const Vector2& position,
    const TextureRegion& textureRegion,
    const Color& color,
    const Radian<float>& rotation,
    const Vector2& originPivot,
    float scale)
{
    POMDOG_ASSERT(impl);
    auto offset = ComputeSpriteOffset(textureRegion, originPivot);
    constexpr float layerDepth = 0.0f;
    impl->Draw(texture, position, textureRegion.subrect, color, rotation, offset, {scale, scale}, layerDepth);
}

void SpriteBatch::Draw(
    const std::shared_ptr<gpu::RenderTarget2D>& texture,
    const Vector2& position,
    const TextureRegion& textureRegion,
    const Color& color,
    const Radian<float>& rotation,
    const Vector2& originPivot,
    const Vector2& scale)
{
    POMDOG_ASSERT(impl);
    auto offset = ComputeSpriteOffset(textureRegion, originPivot);
    constexpr float layerDepth = 0.0f;
    impl->Draw(texture, position, textureRegion.subrect, color, rotation, offset, scale, layerDepth);
}

int SpriteBatch::GetDrawCallCount() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->drawCallCount;
}

} // namespace pomdog
