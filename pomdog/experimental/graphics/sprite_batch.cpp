// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/shader_loader.h"
#include "pomdog/experimental/texture_packer/texture_region.h"
#include "pomdog/gpu/blend_desc.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/depth_stencil_desc.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/index_format.h"
#include "pomdog/gpu/input_layout_helper.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/rasterizer_desc.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/sampler_desc.h"
#include "pomdog/gpu/sampler_state.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_pipeline_stage.h"
#include "pomdog/gpu/texture.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/math/color.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"
#include "pomdog/memory/aligned_new.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstring>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::memory::AlignedNew;

namespace pomdog {
namespace {

Vector2 ComputeSpriteOffset(const TextureRegion& region, const Vector2& originPivot) noexcept
{
    if ((region.subrect.width <= 0) || (region.subrect.height <= 0)) {
        return Vector2::createZero();
    }

    POMDOG_ASSERT(region.subrect.width > 0);
    POMDOG_ASSERT(region.subrect.height > 0);

    const auto regionSize = Vector2{
        static_cast<f32>(region.width),
        static_cast<f32>(region.height)};

    const auto baseOffset = regionSize * originPivot;

    const auto w = static_cast<f32>(region.subrect.width);
    const auto h = static_cast<f32>(region.subrect.height);

    auto offset = Vector2{
        static_cast<f32>(region.xOffset),
        regionSize.y - (static_cast<f32>(region.yOffset) + h)};
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
    static constexpr u32 MaxBatchSize = 2048;
    static constexpr u32 MinBatchSize = 128;
    static constexpr u32 MaxDrawCallCount = 16;

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
    std::shared_ptr<gpu::Texture> currentTexture;

    std::shared_ptr<gpu::VertexBuffer> planeVertices;
    std::shared_ptr<gpu::IndexBuffer> planeIndices;
    std::shared_ptr<gpu::VertexBuffer> instanceVertices;

    std::shared_ptr<gpu::PipelineState> pipelineState;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer;
    std::shared_ptr<gpu::SamplerState> sampler_;

    Vector2 inverseTextureSize;
    u32 startInstanceLocation_ = 0;

public:
    u32 drawCallCount_ = 0;

public:
    Impl() = default;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<gpu::BlendDesc>&& blendState,
        std::optional<gpu::RasterizerDesc>&& rasterizerDesc,
        std::optional<gpu::SamplerDesc>&& samplerState,
        std::optional<gpu::PixelFormat>&& renderTargetViewFormat,
        std::optional<gpu::PixelFormat>&& depthStencilViewFormat,
        SpriteBatchPixelShaderMode pixelShaderMode);

    void Begin(
        const std::shared_ptr<gpu::CommandList>& commandListIn,
        const Matrix4x4& transformMatrix,
        std::optional<SpriteBatchDistanceFieldParameters>&& distanceFieldParameters);

    void Draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale,
        f32 layerDepth);

    void FlushBatch();

    void End();

private:
    void RenderBatch(
        const std::shared_ptr<gpu::Texture>& texture,
        const std::vector<SpriteInfo>& sprites);

    void CompareTexture(const std::shared_ptr<gpu::Texture>& texture);
};

std::unique_ptr<Error>
SpriteBatch::Impl::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::BlendDesc>&& blendDesc,
    std::optional<gpu::RasterizerDesc>&& rasterizerDesc,
    std::optional<gpu::SamplerDesc>&& samplerDesc,
    std::optional<gpu::PixelFormat>&& renderTargetViewFormat,
    std::optional<gpu::PixelFormat>&& depthStencilViewFormat,
    SpriteBatchPixelShaderMode pixelShaderMode)
{
    auto presentationParameters = graphicsDevice->getPresentationParameters();

    if (!blendDesc) {
        blendDesc = gpu::BlendDesc::createNonPremultiplied();
    }
    if (!rasterizerDesc) {
        rasterizerDesc = gpu::RasterizerDesc::createCullNone();
    }
    if (!samplerDesc) {
        samplerDesc = gpu::SamplerDesc::createLinearClamp();
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
        if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
                verticesCombo.data(),
                static_cast<u32>(verticesCombo.size()),
                sizeof(PositionTextureCoord),
                gpu::BufferUsage::Immutable);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
        else {
            planeVertices = std::move(buffer);
        }
    }
    {
        std::array<u16, 6> const indices = {{0, 1, 2, 2, 3, 0}};

        // Create index buffer
        if (auto [buffer, err] = graphicsDevice->createIndexBuffer(
                gpu::IndexFormat::UInt16,
                indices.data(),
                static_cast<u32>(indices.size()),
                gpu::BufferUsage::Immutable);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create index buffer");
        }
        else {
            planeIndices = std::move(buffer);
        }
    }
    {
        const auto maxBatchSize = MaxBatchSize;
        if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
                maxBatchSize,
                sizeof(SpriteInfo),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create instance vertex buffer");
        }
        else {
            instanceVertices = std::move(buffer);
        }
    }
    {
        if (auto [buffer, err] = graphicsDevice->createConstantBuffer(
                sizeof(SpriteBatchConstantBuffer),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create constant buffer");
        }
        else {
            constantBuffer = std::move(buffer);
        }

        if (auto [sampler, err] = graphicsDevice->createSamplerState(
                *samplerDesc);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sampler state");
        }
        else {
            sampler_ = std::move(sampler);
        }
    }
    {
        auto inputLayout = gpu::InputLayoutHelper{}
                               .addInputSlot()
                               .addFloat4()
                               .addInputSlot(gpu::InputClassification::InputPerInstance, 1)
                               .addFloat4()
                               .addFloat4()
                               .addFloat4()
                               .addFloat4()
                               .addFloat4();

        auto [vertexShader, vsErr] = loadShaderAutomagically(
            fs, graphicsDevice,
            gpu::ShaderPipelineStage::VertexShader,
            "/assets/shaders", "sprite_batch_vs", "sprite_batch_vs");
        if (vsErr != nullptr) {
            return errors::wrap(std::move(vsErr), "failed to load vertex shader");
        }

        std::string psName;
        switch (pixelShaderMode) {
        case SpriteBatchPixelShaderMode::Default:
            psName = "sprite_batch_ps";
            break;
        case SpriteBatchPixelShaderMode::DistanceField:
            psName = "sprite_batch_distance_field_ps";
            break;
        }

        auto [pixelShader, psErr] = loadShaderAutomagically(
            fs, graphicsDevice,
            gpu::ShaderPipelineStage::PixelShader,
            "/assets/shaders", psName, psName);
        if (psErr != nullptr) {
            return errors::wrap(std::move(psErr), "failed to load pixel shader");
        }

        auto pipelineStateBuilder = PipelineStateBuilder(graphicsDevice);
        pipelineStateBuilder.setRenderTargetViewFormat(*renderTargetViewFormat);
        pipelineStateBuilder.setDepthStencilViewFormat(*depthStencilViewFormat);
        pipelineStateBuilder.setVertexShader(std::move(vertexShader));
        pipelineStateBuilder.setPixelShader(std::move(pixelShader));
        pipelineStateBuilder.setInputLayout(inputLayout.createInputLayout());
        pipelineStateBuilder.setPrimitiveTopology(gpu::PrimitiveTopology::TriangleList);
        pipelineStateBuilder.setBlendState(*blendDesc);
        pipelineStateBuilder.setDepthStencilState(gpu::DepthStencilDesc::createNone());
        pipelineStateBuilder.setRasterizerState(*rasterizerDesc);

        auto [pipeline, pipelineErr] = pipelineStateBuilder.build();
        if (pipelineErr != nullptr) {
            return errors::wrap(std::move(pipelineErr), "failed to create pipeline state");
        }
        pipelineState = std::move(pipeline);
    }

    spriteQueue.reserve(MinBatchSize);

    return nullptr;
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
        constants.DistanceFieldParameters.y = 0.45f;
    }

    constantBuffer->setData(0, gpu::makeByteSpan(constants));

    startInstanceLocation_ = 0;
    drawCallCount_ = 0;
}

void SpriteBatch::Impl::End()
{
    FlushBatch();

    if (drawCallCount_ > 0) {
        commandList->setTexture(0);
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
    POMDOG_ASSERT((startInstanceLocation_ + spriteQueue.size()) <= MaxBatchSize);

    RenderBatch(currentTexture, spriteQueue);

    currentTexture = nullptr;
    spriteQueue.clear();
}

void SpriteBatch::Impl::RenderBatch(
    const std::shared_ptr<gpu::Texture>& texture,
    const std::vector<SpriteInfo>& sprites)
{
    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(!sprites.empty());
    POMDOG_ASSERT((startInstanceLocation_ + sprites.size()) <= MaxBatchSize);

    static_assert(std::is_unsigned_v<decltype(drawCallCount_)>, "drawCallCount_ >= 0");

    POMDOG_ASSERT(sprites.size() <= MaxBatchSize);
    const auto instanceOffsetBytes = static_cast<u32>(sizeof(SpriteInfo) * startInstanceLocation_);
    instanceVertices->setData(
        instanceOffsetBytes,
        sprites.data(),
        static_cast<u32>(sprites.size()),
        sizeof(SpriteInfo));

    commandList->setTexture(0, texture);
    commandList->setSamplerState(0, sampler_);

    commandList->setPipelineState(pipelineState);
    commandList->setConstantBuffer(0, constantBuffer);
    commandList->setVertexBuffer(0, planeVertices);
    commandList->setVertexBuffer(1, instanceVertices);
    commandList->setIndexBuffer(planeIndices);

    commandList->drawIndexedInstanced(
        planeIndices->getIndexCount(),
        static_cast<u32>(sprites.size()),
        0,
        startInstanceLocation_);

    startInstanceLocation_ += static_cast<u32>(sprites.size());
    POMDOG_ASSERT(startInstanceLocation_ <= MaxBatchSize);

    ++drawCallCount_;
}

void SpriteBatch::Impl::CompareTexture(const std::shared_ptr<gpu::Texture>& texture)
{
    POMDOG_ASSERT(texture != nullptr);

    if (texture != currentTexture) {
        if (currentTexture != nullptr) {
            FlushBatch();
        }
        POMDOG_ASSERT(spriteQueue.empty());
        POMDOG_ASSERT(currentTexture == nullptr);

        currentTexture = texture;

        POMDOG_ASSERT(texture->getWidth() > 0);
        POMDOG_ASSERT(texture->getHeight() > 0);

        const f32 w = static_cast<f32>(texture->getWidth());
        const f32 h = static_cast<f32>(texture->getHeight());

        inverseTextureSize.x = (w > 0.0f) ? (1.0f / w) : 0.0f;
        inverseTextureSize.y = (h > 0.0f) ? (1.0f / h) : 0.0f;
    }
}

void SpriteBatch::Impl::Draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    const Vector2& scale,
    f32 layerDepth)
{
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(texture->getWidth() > 0);
    POMDOG_ASSERT(texture->getHeight() > 0);
    POMDOG_ASSERT(sourceRect.width >= 0);
    POMDOG_ASSERT(sourceRect.height >= 0);

    if ((sourceRect.width == 0) || (sourceRect.height == 0)) {
        return;
    }

    if ((scale.x == 0.0f) || (scale.y == 0.0f)) {
        return;
    }

    if ((startInstanceLocation_ + spriteQueue.size()) >= MaxBatchSize) {
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

    switch (texture->getFormat()) {
    case gpu::PixelFormat::R8_UNorm:
    case gpu::PixelFormat::R8G8_UNorm:
    case gpu::PixelFormat::R16G16_Float:
    case gpu::PixelFormat::R11G11B10_Float:
    case gpu::PixelFormat::R32_Float:
        sourceAlphaEnabled = false;
        compensationAlpha = true;
        break;
    case gpu::PixelFormat::A8_UNorm:
        sourceRGBEnabled = false;
        compensationRGB = true;
        break;
    case gpu::PixelFormat::Invalid:
    case gpu::PixelFormat::R8G8B8A8_UNorm:
    case gpu::PixelFormat::R10G10B10A2_UNorm:
    case gpu::PixelFormat::B8G8R8A8_UNorm:
    case gpu::PixelFormat::R16G16B16A16_Float:
    case gpu::PixelFormat::R32G32B32A32_Float:
    case gpu::PixelFormat::BlockComp1_UNorm:
    case gpu::PixelFormat::BlockComp2_UNorm:
    case gpu::PixelFormat::BlockComp3_UNorm:
    case gpu::PixelFormat::Depth16:
    case gpu::PixelFormat::Depth32:
    case gpu::PixelFormat::Depth24Stencil8:
    case gpu::PixelFormat::Depth32_Float_Stencil8_Uint:
        break;
    }

    const int colorModeFlags =
        (sourceRGBEnabled ? 1 : 0) |
        (sourceAlphaEnabled ? 2 : 0) |
        (compensationRGB ? 4 : 0) |
        (compensationAlpha ? 8 : 0);

    POMDOG_ASSERT((startInstanceLocation_ + spriteQueue.size()) < MaxBatchSize);
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
        static_cast<f32>(sourceRect.x),
        static_cast<f32>(sourceRect.y),
        static_cast<f32>(sourceRect.width),
        static_cast<f32>(sourceRect.height),
    };
    info.OriginRotationLayerDepth = Vector4{
        originPivot.x,
        originPivot.y,
        rotation.get(),
        layerDepth,
    };
    info.Color = color.toVector4();
    info.InverseTextureSize = Vector4{
        inverseTextureSize.x,
        inverseTextureSize.y,
        static_cast<f32>(colorModeFlags),
        0.0f,
    };

    spriteQueue.push_back(std::move(info));
    POMDOG_ASSERT((startInstanceLocation_ + spriteQueue.size()) <= MaxBatchSize);
}

// MARK: - SpriteBatch

SpriteBatch::SpriteBatch() = default;

SpriteBatch::~SpriteBatch() = default;

std::unique_ptr<Error>
SpriteBatch::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    return initialize(
        fs,
        graphicsDevice,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        SpriteBatchPixelShaderMode::Default);
}

std::unique_ptr<Error>
SpriteBatch::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::BlendDesc>&& blendDesc,
    std::optional<gpu::RasterizerDesc>&& rasterizerDesc,
    std::optional<gpu::SamplerDesc>&& samplerDesc,
    std::optional<gpu::PixelFormat>&& renderTargetViewFormat,
    std::optional<gpu::PixelFormat>&& depthStencilViewFormat,
    SpriteBatchPixelShaderMode pixelShaderMode)
{
    impl = std::make_unique<Impl>();
    return impl->initialize(
        fs,
        graphicsDevice,
        std::move(blendDesc),
        std::move(rasterizerDesc),
        std::move(samplerDesc),
        std::move(renderTargetViewFormat),
        std::move(depthStencilViewFormat),
        pixelShaderMode);
}

void SpriteBatch::begin(
    const std::shared_ptr<gpu::CommandList>& commandList,
    const Matrix4x4& transformMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->Begin(commandList, transformMatrixIn, std::nullopt);
}

void SpriteBatch::begin(
    const std::shared_ptr<gpu::CommandList>& commandList,
    const Matrix4x4& transformMatrixIn,
    const SpriteBatchDistanceFieldParameters& distanceFieldParameters)
{
    POMDOG_ASSERT(impl);
    impl->Begin(commandList, transformMatrixIn, distanceFieldParameters);
}

void SpriteBatch::flush()
{
    POMDOG_ASSERT(impl);
    impl->FlushBatch();
}

void SpriteBatch::end()
{
    POMDOG_ASSERT(impl);
    impl->End();
}

void SpriteBatch::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Rect2D& sourceRect,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    constexpr f32 layerDepth = 0.0f;
    impl->Draw(texture, {0, 0}, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, layerDepth);
}

void SpriteBatch::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    constexpr f32 layerDepth = 0.0f;
    const Rect2D sourceRect = {0, 0, texture->getWidth(), texture->getHeight()};
    impl->Draw(texture, position, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, layerDepth);
}

void SpriteBatch::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const Color& color)
{
    POMDOG_ASSERT(impl);
    constexpr f32 layerDepth = 0.0f;
    impl->Draw(texture, position, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, layerDepth);
}

void SpriteBatch::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    f32 scale)
{
    POMDOG_ASSERT(impl);
    constexpr f32 layerDepth = 0.0f;
    impl->Draw(texture, position, sourceRect, color, rotation, originPivot, {scale, scale}, layerDepth);
}

void SpriteBatch::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    const Vector2& scale)
{
    POMDOG_ASSERT(impl);
    constexpr f32 layerDepth = 0.0f;
    impl->Draw(texture, position, sourceRect, color, rotation, originPivot, scale, layerDepth);
}

void SpriteBatch::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const TextureRegion& textureRegion,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    f32 scale)
{
    POMDOG_ASSERT(impl);
    auto offset = ComputeSpriteOffset(textureRegion, originPivot);
    constexpr f32 layerDepth = 0.0f;
    impl->Draw(texture, position, textureRegion.subrect, color, rotation, offset, {scale, scale}, layerDepth);
}

void SpriteBatch::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const TextureRegion& textureRegion,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    const Vector2& scale)
{
    POMDOG_ASSERT(impl);
    auto offset = ComputeSpriteOffset(textureRegion, originPivot);
    constexpr f32 layerDepth = 0.0f;
    impl->Draw(texture, position, textureRegion.subrect, color, rotation, offset, scale, layerDepth);
}

u32 SpriteBatch::getDrawCallCount() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->drawCallCount_;
}

} // namespace pomdog
