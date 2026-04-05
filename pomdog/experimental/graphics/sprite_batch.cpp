// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/basic/conditional_compilation.h"
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
#include "pomdog/gpu/input_layout_builder.h"
#include "pomdog/gpu/pipeline_desc.h"
#include "pomdog/gpu/pipeline_state.h"
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
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstring>
#include <limits>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::memory::AlignedNew;

namespace pomdog {
namespace {

[[nodiscard]] Vector2
computeSpriteOffset(const TextureRegion& region, const Vector2& originPivot) noexcept
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
    // {__z_} = OutlineWeight
    // {___w} = unused
    Vector4 DistanceFieldParameters;

    // {rgba} = OutlineColor
    Vector4 OutlineColor;
};

class SpritePipelineImpl final : public SpritePipeline {
public:
    std::shared_ptr<gpu::VertexBuffer> planeVertices_;
    std::shared_ptr<gpu::IndexBuffer> planeIndices_;
    std::shared_ptr<gpu::PipelineState> pipelineState_;
    std::shared_ptr<gpu::SamplerState> sampler_;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<gpu::BlendDesc>&& blendDesc,
        std::optional<gpu::RasterizerDesc>&& rasterizerDesc,
        std::optional<gpu::SamplerDesc>&& samplerDesc,
        std::optional<gpu::PixelFormat>&& renderTargetViewFormat,
        std::optional<gpu::PixelFormat>&& depthStencilViewFormat,
        SpriteBatchPixelShaderMode pixelShaderMode);
};

std::unique_ptr<Error>
SpritePipelineImpl::initialize(
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
            planeVertices_ = std::move(buffer);
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
            planeIndices_ = std::move(buffer);
        }
    }
    {
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
        std::string vsName;
        switch (pixelShaderMode) {
        case SpriteBatchPixelShaderMode::WaterLine:
            vsName = "sprite_batch_waterline_vs";
            break;
        case SpriteBatchPixelShaderMode::Sprite:
        case SpriteBatchPixelShaderMode::SolidFill:
        case SpriteBatchPixelShaderMode::DistanceField:
        case SpriteBatchPixelShaderMode::DistanceFieldWithOutline:
            vsName = "sprite_batch_vs";
            break;
        }

        auto [vertexShader, vsErr] = loadShaderAutomagically(
            fs, graphicsDevice,
            gpu::ShaderPipelineStage::VertexShader,
            "/assets/shaders", vsName, vsName);
        if (vsErr != nullptr) {
            return errors::wrap(std::move(vsErr), "failed to load vertex shader");
        }

        std::string psName;
        switch (pixelShaderMode) {
        case SpriteBatchPixelShaderMode::Sprite:
            psName = "sprite_batch_ps";
            break;
        case SpriteBatchPixelShaderMode::SolidFill:
            psName = "sprite_batch_solid_fill_ps";
            break;
        case SpriteBatchPixelShaderMode::WaterLine:
            psName = "sprite_batch_waterline_ps";
            break;
        case SpriteBatchPixelShaderMode::DistanceField:
            psName = "sprite_batch_distance_field_ps";
            break;
        case SpriteBatchPixelShaderMode::DistanceFieldWithOutline:
            psName = "sprite_batch_distance_field_outline_ps";
            break;
        }

        auto [pixelShader, psErr] = loadShaderAutomagically(
            fs, graphicsDevice,
            gpu::ShaderPipelineStage::PixelShader,
            "/assets/shaders", psName, psName);
        if (psErr != nullptr) {
            return errors::wrap(std::move(psErr), "failed to load pixel shader");
        }

        gpu::PipelineDesc pipelineDesc = {};
        pipelineDesc.renderTargetViewFormats = {*renderTargetViewFormat};
        pipelineDesc.depthStencilViewFormat = *depthStencilViewFormat;
        pipelineDesc.vertexShader = std::move(vertexShader);
        pipelineDesc.pixelShader = std::move(pixelShader);
        gpu::InputLayoutBuilder::addVertex(pipelineDesc.inputLayout,
            0, gpu::InputClassification::PerVertex, 16,
            {
                gpu::InputElementFormat::Float32x4,
            });
        gpu::InputLayoutBuilder::addVertex(pipelineDesc.inputLayout,
            1, gpu::InputClassification::PerInstance, 64,
            {
                gpu::InputElementFormat::Float32x4,
                gpu::InputElementFormat::Float32x4,
                gpu::InputElementFormat::Float32x4,
                gpu::InputElementFormat::Unorm8x4,
                gpu::InputElementFormat::Unorm8x4,
                gpu::InputElementFormat::Unorm8x4,
                gpu::InputElementFormat::Unorm8x4,
            });
        pipelineDesc.primitiveTopology = gpu::PrimitiveTopology::TriangleList;
        pipelineDesc.blendState = *blendDesc;
        pipelineDesc.depthStencilState = gpu::DepthStencilDesc::createNone();
        pipelineDesc.rasterizerState = *rasterizerDesc;
        pipelineDesc.multiSampleMask = std::numeric_limits<u32>::max();

        auto [pipeline, pipelineErr] = graphicsDevice->createPipelineState(pipelineDesc);
        if (pipelineErr != nullptr) {
            return errors::wrap(std::move(pipelineErr), "failed to create pipeline state");
        }
        pipelineState_ = std::move(pipeline);
    }

    return nullptr;
}

class SpriteBatchImpl final : public SpriteBatch {
private:
    static constexpr u32 MaxBatchSize = 2048;
    static constexpr u32 MinBatchSize = 128;

    static_assert(MaxBatchSize >= MinBatchSize);

    struct alignas(16) SpriteInstance final : public AlignedNew<SpriteInstance> {
        // {xy__} = position.xy
        // {__zw} = scale.xy * sourceRect.{width, height}
        Vector4 translation;

        // {xy__} = {rect.xy * inverseTextureSize}
        // {__zw} = {rect.width * inverseTextureSize, rect.height * inverseTextureSize}
        Vector4 sourceRect;

        // {xy__} = originPivot.xy
        // {__z_} = rotation
        // {___w} = waterLineYPosition
        Vector4 originRotationWaterLine;

        // {rgba} = color0.rgba
        Color color0;

        // {rgba} = color1.rgba
        Color color1;

        // {rgba} = color2.rgba
        Color color2;

        // {xy__} = reserved (unorm8x2)
        // {__z_} = blendFactor0 (unorm8x1)
        // {___w} = blendFactor1 (unorm8x1)
        Color reservedBlendFactor;
    };

    static_assert(sizeof(SpriteInstance) == 64);

    std::vector<SpriteInstance> spriteQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    std::shared_ptr<gpu::Texture> currentTexture_;
    std::shared_ptr<gpu::VertexBuffer> instanceVertices_;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer_;
    unsafe_ptr<SpritePipelineImpl> currentPipeline_ = nullptr;
    Vector2 inverseTextureSize_;
    u32 startInstanceLocation_ = 0;
    u32 drawCallCount_ = 0;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    void begin(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<SpritePipeline>& spritePipeline,
        const Matrix4x4& transformMatrix) override;

    void begin(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<SpritePipeline>& spritePipeline,
        const Matrix4x4& transformMatrix,
        const SpriteBatchDistanceFieldParameters& distanceFieldParameters) override;

    void draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Rect2D& sourceRect,
        const Color& color) override;

    void draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Color& color) override;

    void draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color) override;

    void draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        f32 scale) override;

    void draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale) override;

    void draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        f32 scale) override;

    void draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale) override;

    void flush() override;

    void end() override;

    [[nodiscard]] u32
    getDrawCallCount() const noexcept override;

private:
    void beginImpl(
        const std::shared_ptr<gpu::CommandList>& commandListIn,
        const std::shared_ptr<SpritePipeline>& spritePipeline,
        const Matrix4x4& transformMatrix,
        std::optional<SpriteBatchDistanceFieldParameters>&& distanceFieldParameters);

    void drawImpl(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale,
        f32 layerDepth);

    void flushBatch();

    void renderBatch(
        const std::shared_ptr<gpu::Texture>& texture,
        const std::vector<SpriteInstance>& sprites);

    void compareTexture(const std::shared_ptr<gpu::Texture>& texture);
};

std::unique_ptr<Error>
SpriteBatchImpl::initialize(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    {
        const auto maxBatchSize = MaxBatchSize;
        if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
                maxBatchSize,
                sizeof(SpriteInstance),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create instance vertex buffer");
        }
        else {
            instanceVertices_ = std::move(buffer);
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
            constantBuffer_ = std::move(buffer);
        }
    }

    spriteQueue_.reserve(MinBatchSize);

    return nullptr;
}

void SpriteBatchImpl::beginImpl(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const std::shared_ptr<SpritePipeline>& spritePipeline,
    const Matrix4x4& transformMatrix,
    std::optional<SpriteBatchDistanceFieldParameters>&& distanceFieldParameters)
{
    POMDOG_ASSERT(commandListIn);
    POMDOG_ASSERT(spritePipeline);
    commandList_ = commandListIn;
    currentPipeline_ = static_cast<SpritePipelineImpl*>(spritePipeline.get());

    POMDOG_ASSERT(constantBuffer_);

    SpriteBatchConstantBuffer constants;
    constants.ViewProjection = math::transpose(transformMatrix);

    if (distanceFieldParameters != std::nullopt) {
        constants.DistanceFieldParameters.x = distanceFieldParameters->Smoothing;
        constants.DistanceFieldParameters.y = distanceFieldParameters->Weight;
        constants.DistanceFieldParameters.z = distanceFieldParameters->OutlineWeight;
        constants.DistanceFieldParameters.w = 0.0f;
        constants.OutlineColor = distanceFieldParameters->OutlineColor.toVector4();
    }
    else {
        constants.DistanceFieldParameters.x = 0.25f;
        constants.DistanceFieldParameters.y = 0.45f;
        constants.DistanceFieldParameters.z = 0.5f;
        constants.DistanceFieldParameters.w = 0.0f;
        constants.OutlineColor = Vector4{0.0f, 0.0f, 0.0f, 0.0f};
    }

    constantBuffer_->setData(0, gpu::makeByteSpan(constants));

    startInstanceLocation_ = 0;
    drawCallCount_ = 0;
}

void SpriteBatchImpl::end()
{
    flushBatch();

    if (drawCallCount_ > 0) {
        commandList_->setTexture(0);
    }
    commandList_.reset();
    currentPipeline_ = nullptr;
}

void SpriteBatchImpl::flushBatch()
{
    if (spriteQueue_.empty()) {
        return;
    }

    POMDOG_ASSERT(currentTexture_ != nullptr);
    POMDOG_ASSERT(!spriteQueue_.empty());
    POMDOG_ASSERT((startInstanceLocation_ + spriteQueue_.size()) <= MaxBatchSize);

    renderBatch(currentTexture_, spriteQueue_);

    currentTexture_ = nullptr;
    spriteQueue_.clear();
}

void SpriteBatchImpl::renderBatch(
    const std::shared_ptr<gpu::Texture>& texture,
    const std::vector<SpriteInstance>& sprites)
{
    POMDOG_ASSERT(commandList_);
    POMDOG_ASSERT(currentPipeline_);
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(!sprites.empty());
    POMDOG_ASSERT((startInstanceLocation_ + sprites.size()) <= MaxBatchSize);

    static_assert(std::is_unsigned_v<decltype(drawCallCount_)>, "drawCallCount_ >= 0");

    POMDOG_ASSERT(sprites.size() <= MaxBatchSize);
    const auto instanceOffsetBytes = static_cast<u32>(sizeof(SpriteInstance) * startInstanceLocation_);
    instanceVertices_->setData(
        instanceOffsetBytes,
        sprites.data(),
        static_cast<u32>(sprites.size()),
        sizeof(SpriteInstance));

    commandList_->setTexture(0, texture);
    commandList_->setSamplerState(0, currentPipeline_->sampler_);

    commandList_->setPipelineState(currentPipeline_->pipelineState_);
    commandList_->setConstantBuffer(0, constantBuffer_);
    commandList_->setVertexBuffer(0, currentPipeline_->planeVertices_);
    commandList_->setVertexBuffer(1, instanceVertices_);
    commandList_->setIndexBuffer(currentPipeline_->planeIndices_);

    commandList_->drawIndexedInstanced(
        currentPipeline_->planeIndices_->getIndexCount(),
        static_cast<u32>(sprites.size()),
        0,
        startInstanceLocation_);

    startInstanceLocation_ += static_cast<u32>(sprites.size());
    POMDOG_ASSERT(startInstanceLocation_ <= MaxBatchSize);

    ++drawCallCount_;
}

void SpriteBatchImpl::compareTexture(const std::shared_ptr<gpu::Texture>& texture)
{
    POMDOG_ASSERT(texture != nullptr);

    if (texture != currentTexture_) {
        if (currentTexture_ != nullptr) {
            flushBatch();
        }
        POMDOG_ASSERT(spriteQueue_.empty());
        POMDOG_ASSERT(currentTexture_ == nullptr);

        currentTexture_ = texture;

        POMDOG_ASSERT(texture->getWidth() > 0);
        POMDOG_ASSERT(texture->getHeight() > 0);

        const f32 w = static_cast<f32>(texture->getWidth());
        const f32 h = static_cast<f32>(texture->getHeight());

        inverseTextureSize_.x = (w > 0.0f) ? (1.0f / w) : 0.0f;
        inverseTextureSize_.y = (h > 0.0f) ? (1.0f / h) : 0.0f;
    }
}

void SpriteBatchImpl::drawImpl(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    const Vector2& scale,
    [[maybe_unused]] f32 layerDepth)
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

    if ((startInstanceLocation_ + spriteQueue_.size()) >= MaxBatchSize) {
        flushBatch();
        POMDOG_ASSERT(spriteQueue_.empty());

        // TODO: Not implemented
        // GrowSpriteQueue();
        return;
    }

    POMDOG_ASSERT((startInstanceLocation_ + spriteQueue_.size()) < MaxBatchSize);
    POMDOG_ASSERT(sourceRect.width > 0);
    POMDOG_ASSERT(sourceRect.height > 0);

    compareTexture(texture);

    SpriteInstance info = {};
    info.translation = Vector4{
        position.x,
        position.y,
        scale.x * static_cast<f32>(sourceRect.width),
        scale.y * static_cast<f32>(sourceRect.height),
    };
    info.sourceRect = Vector4{
        static_cast<f32>(sourceRect.x) * inverseTextureSize_.x,
        static_cast<f32>(sourceRect.y) * inverseTextureSize_.y,
        static_cast<f32>(sourceRect.width) * inverseTextureSize_.x,
        static_cast<f32>(sourceRect.height) * inverseTextureSize_.y,
    };
    info.originRotationWaterLine = Vector4{
        originPivot.x,
        originPivot.y,
        rotation.get(),
        -30000.0f,
    };
    info.color0 = color;
    info.color1 = Color::createWhite();
    info.color2 = color;
    info.reservedBlendFactor = Color{0, 0, 0, 0};

    spriteQueue_.push_back(std::move(info));
    POMDOG_ASSERT((startInstanceLocation_ + spriteQueue_.size()) <= MaxBatchSize);
}

void SpriteBatchImpl::begin(
    const std::shared_ptr<gpu::CommandList>& commandList,
    const std::shared_ptr<SpritePipeline>& spritePipeline,
    const Matrix4x4& transformMatrixIn)
{
    beginImpl(commandList, spritePipeline, transformMatrixIn, std::nullopt);
}

void SpriteBatchImpl::begin(
    const std::shared_ptr<gpu::CommandList>& commandList,
    const std::shared_ptr<SpritePipeline>& spritePipeline,
    const Matrix4x4& transformMatrixIn,
    const SpriteBatchDistanceFieldParameters& distanceFieldParameters)
{
    beginImpl(commandList, spritePipeline, transformMatrixIn, distanceFieldParameters);
}

void SpriteBatchImpl::flush()
{
    flushBatch();
}

void SpriteBatchImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Rect2D& sourceRect,
    const Color& color)
{
    constexpr f32 layerDepth = 0.0f;
    drawImpl(texture, {0, 0}, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, layerDepth);
}

void SpriteBatchImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Color& color)
{
    constexpr f32 layerDepth = 0.0f;
    const Rect2D sourceRect = {0, 0, texture->getWidth(), texture->getHeight()};
    drawImpl(texture, position, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, layerDepth);
}

void SpriteBatchImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const Color& color)
{
    constexpr f32 layerDepth = 0.0f;
    drawImpl(texture, position, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, layerDepth);
}

void SpriteBatchImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    f32 scale)
{
    constexpr f32 layerDepth = 0.0f;
    drawImpl(texture, position, sourceRect, color, rotation, originPivot, {scale, scale}, layerDepth);
}

void SpriteBatchImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    const Vector2& scale)
{
    constexpr f32 layerDepth = 0.0f;
    drawImpl(texture, position, sourceRect, color, rotation, originPivot, scale, layerDepth);
}

void SpriteBatchImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const TextureRegion& textureRegion,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    f32 scale)
{
    auto offset = computeSpriteOffset(textureRegion, originPivot);
    constexpr f32 layerDepth = 0.0f;
    drawImpl(texture, position, textureRegion.subrect, color, rotation, offset, {scale, scale}, layerDepth);
}

void SpriteBatchImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const TextureRegion& textureRegion,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    const Vector2& scale)
{
    auto offset = computeSpriteOffset(textureRegion, originPivot);
    constexpr f32 layerDepth = 0.0f;
    drawImpl(texture, position, textureRegion.subrect, color, rotation, offset, scale, layerDepth);
}

u32 SpriteBatchImpl::getDrawCallCount() const noexcept
{
    return drawCallCount_;
}

} // namespace

SpritePipeline::~SpritePipeline() = default;

SpriteBatch::~SpriteBatch() = default;

std::tuple<std::shared_ptr<SpritePipeline>, std::unique_ptr<Error>>
createSpritePipeline(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::BlendDesc>&& blendDesc,
    std::optional<gpu::RasterizerDesc>&& rasterizerDesc,
    std::optional<gpu::SamplerDesc>&& samplerDesc,
    std::optional<gpu::PixelFormat>&& renderTargetViewFormat,
    std::optional<gpu::PixelFormat>&& depthStencilViewFormat,
    SpriteBatchPixelShaderMode pixelShaderMode) noexcept
{
    auto pipeline = std::make_shared<SpritePipelineImpl>();
    if (auto err = pipeline->initialize(
            fs, graphicsDevice,
            std::move(blendDesc), std::move(rasterizerDesc), std::move(samplerDesc),
            std::move(renderTargetViewFormat), std::move(depthStencilViewFormat),
            pixelShaderMode);
        err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(pipeline), nullptr);
}

std::tuple<std::shared_ptr<SpriteBatch>, std::unique_ptr<Error>>
createSpriteBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) noexcept
{
    auto spriteBatch = std::make_shared<SpriteBatchImpl>();
    if (auto err = spriteBatch->initialize(graphicsDevice); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(spriteBatch), nullptr);
}

} // namespace pomdog
