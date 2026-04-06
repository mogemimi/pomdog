// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/unreachable.h"
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
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstring>
#include <limits>
#include <numeric>
#include <ranges>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
    Matrix4x4 viewProjection;

    // {x___} = smoothing
    // {_y__} = weight
    // {__z_} = outlineWeight
    // {___w} = unused
    Vector4 distanceFieldParameters;

    // {rgba} = outlineColor
    Vector4 outlineColor;
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

struct SpriteInstance final {
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

class SpriteBatchImpl final : public SpriteBatch {
private:
    static constexpr u32 DefaultBatchSize = 2048;
    static constexpr u32 MinBatchSize = 64;
    static constexpr u32 MaxBatchSize = 65536;

    std::vector<SpriteInstance> instances_;
    std::vector<std::shared_ptr<gpu::Texture>> textures_;
    std::vector<f32> layerDepths_;
    std::shared_ptr<gpu::VertexBuffer> instanceVertices_;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer_;
    u32 maxBatchSize_ = DefaultBatchSize;
    u32 drawCallCount_ = 0;
    u32 nextInstance_ = 0;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<u32> batchSize);

    void reset() override;

    void setTransform(const Matrix4x4& transformMatrix) override;

    void setTransform(
        const Matrix4x4& transformMatrix,
        f32 fontSmoothing,
        f32 fontWeight,
        const Color& outlineColor,
        f32 outlineWeight) override;

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

    void draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const SpriteBatchDrawParameters& params) override;

    void draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const SpriteBatchDrawParameters& params) override;

    void sort(SpriteSortMode sortMode) override;

    void flush(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<SpritePipeline>& spritePipeline) override;

    void submit(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) override;

    [[nodiscard]] u32
    getDrawCallCount() const noexcept override;

private:
    void setTransformImpl(
        const Matrix4x4& transformMatrix,
        f32 fontSmoothing,
        f32 fontWeight,
        const Color& outlineColor,
        f32 outlineWeight);

    void drawImpl(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const SpriteBatchDrawParameters& params);

    void flushBatch(
        const std::shared_ptr<gpu::CommandList>& commandList,
        unsafe_ptr<SpritePipelineImpl> pipeline);

    void renderBatch(
        const std::shared_ptr<gpu::CommandList>& commandList,
        unsafe_ptr<SpritePipelineImpl> pipeline,
        const std::shared_ptr<gpu::Texture>& texture,
        u32 firstInstance,
        u32 instanceCount);
};

std::unique_ptr<Error>
SpriteBatchImpl::initialize(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize)
{
    if (batchSize.has_value()) {
        if (*batchSize < MinBatchSize) {
            return errors::make("batchSize is too small (minimum: " + std::to_string(MinBatchSize) + ")");
        }
        if (*batchSize > MaxBatchSize) {
            return errors::make("batchSize is too large (maximum: " + std::to_string(MaxBatchSize) + ")");
        }
        maxBatchSize_ = *batchSize;
    }

    {
        if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
                maxBatchSize_,
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

    instances_.reserve(MinBatchSize);

    return nullptr;
}

void SpriteBatchImpl::reset()
{
    instances_.clear();
    textures_.clear();
    layerDepths_.clear();
    nextInstance_ = 0;
    drawCallCount_ = 0;
}

void SpriteBatchImpl::setTransform(const Matrix4x4& transformMatrix)
{
    setTransformImpl(transformMatrix, 0.25f, 0.45f, Color::createTransparentBlack(), 0.5f);
}

void SpriteBatchImpl::setTransform(
    const Matrix4x4& transformMatrix,
    f32 fontSmoothing,
    f32 fontWeight,
    const Color& outlineColor,
    f32 outlineWeight)
{
    setTransformImpl(transformMatrix, fontSmoothing, fontWeight, outlineColor, outlineWeight);
}

void SpriteBatchImpl::setTransformImpl(
    const Matrix4x4& transformMatrix,
    f32 fontSmoothing,
    f32 fontWeight,
    const Color& outlineColor,
    f32 outlineWeight)
{
    POMDOG_ASSERT(constantBuffer_);

    SpriteBatchConstantBuffer constants;
    constants.viewProjection = math::transpose(transformMatrix);
    constants.distanceFieldParameters.x = fontSmoothing;
    constants.distanceFieldParameters.y = fontWeight;
    constants.distanceFieldParameters.z = outlineWeight;
    constants.distanceFieldParameters.w = 0.0f;
    constants.outlineColor = outlineColor.toVector4();

    constantBuffer_->setData(0, gpu::makeByteSpan(constants));
}

void SpriteBatchImpl::flushBatch(
    const std::shared_ptr<gpu::CommandList>& commandList,
    unsafe_ptr<SpritePipelineImpl> pipeline)
{
    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(pipeline);
    POMDOG_ASSERT(instances_.size() == textures_.size());

    const auto maxInstanceCount = std::min(maxBatchSize_, static_cast<u32>(textures_.size()));
    if (nextInstance_ >= maxInstanceCount) {
        return;
    }

    // NOTE: Iterate through instances, batching by consecutive same-texture runs.
    std::shared_ptr<gpu::Texture> batchTexture = textures_[nextInstance_];
    u32 batchStart = nextInstance_;

    for (u32 i = nextInstance_ + 1; i <= maxInstanceCount; ++i) {
        if (i == maxInstanceCount || textures_[i] != batchTexture) {
            renderBatch(commandList, pipeline, batchTexture, batchStart, i - batchStart);
            if (i < maxInstanceCount) {
                batchTexture = textures_[i];
                batchStart = i;
            }
        }
    }

    nextInstance_ = maxInstanceCount;
}

void SpriteBatchImpl::submit(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    // NOTE: Grow the vertex buffer if needed (takes effect on next frame).
    [&] {
        const auto instanceCount = static_cast<u32>(instances_.size());
        if (instanceCount <= maxBatchSize_) {
            return;
        }

        if (graphicsDevice == nullptr) {
            return;
        }

        constexpr u32 divisor = 2048;
        const auto requiredSize = ((instanceCount + divisor - 1) / divisor) * divisor;

        if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
                requiredSize,
                sizeof(SpriteInstance),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            // NOTE: Failed to resize; sprites beyond maxBatchSize_ are dropped this frame.
        }
        else {
            instanceVertices_ = std::move(buffer);
            maxBatchSize_ = requiredSize;
        }
    }();

    // NOTE: Upload instance data to GPU.
    if (!instances_.empty()) {
        const auto instanceCount = std::min(maxBatchSize_, static_cast<u32>(instances_.size()));
        instanceVertices_->setData(
            0,
            instances_.data(),
            instanceCount,
            sizeof(SpriteInstance));
    }

    // NOTE: Shrink over-allocated CPU vectors.
    if (instances_.capacity() > maxBatchSize_) {
        instances_.resize(maxBatchSize_);
        instances_.shrink_to_fit();
    }
    if (textures_.capacity() > maxBatchSize_) {
        textures_.resize(maxBatchSize_);
        textures_.shrink_to_fit();
    }
    if (layerDepths_.capacity() > maxBatchSize_) {
        layerDepths_.resize(maxBatchSize_);
        layerDepths_.shrink_to_fit();
    }
}

void SpriteBatchImpl::renderBatch(
    const std::shared_ptr<gpu::CommandList>& commandList,
    unsafe_ptr<SpritePipelineImpl> pipeline,
    const std::shared_ptr<gpu::Texture>& texture,
    u32 firstInstance,
    u32 instanceCount)
{
    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(pipeline);
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(instanceCount > 0);

    commandList->setTexture(0, texture);
    commandList->setSamplerState(0, pipeline->sampler_);

    commandList->setPipelineState(pipeline->pipelineState_);
    commandList->setConstantBuffer(0, constantBuffer_);
    commandList->setVertexBuffer(0, pipeline->planeVertices_);
    commandList->setVertexBuffer(1, instanceVertices_);
    commandList->setIndexBuffer(pipeline->planeIndices_);

    commandList->drawIndexedInstanced(
        pipeline->planeIndices_->getIndexCount(),
        instanceCount,
        0,
        firstInstance);

    ++drawCallCount_;
}

void SpriteBatchImpl::drawImpl(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const SpriteBatchDrawParameters& params)
{
    POMDOG_ASSERT(texture != nullptr);
    POMDOG_ASSERT(texture->getWidth() > 0);
    POMDOG_ASSERT(texture->getHeight() > 0);
    POMDOG_ASSERT(sourceRect.width >= 0);
    POMDOG_ASSERT(sourceRect.height >= 0);

    if (texture == nullptr) [[unlikely]] {
        return;
    }

    if ((sourceRect.width == 0) || (sourceRect.height == 0)) [[unlikely]] {
        return;
    }

    if ((params.scale.x == 0.0f) || (params.scale.y == 0.0f)) [[unlikely]] {
        return;
    }

    POMDOG_ASSERT(sourceRect.width > 0);
    POMDOG_ASSERT(sourceRect.height > 0);

    const f32 texW = static_cast<f32>(texture->getWidth());
    const f32 texH = static_cast<f32>(texture->getHeight());
    const f32 invW = (texW > 0.0f) ? (1.0f / texW) : 0.0f;
    const f32 invH = (texH > 0.0f) ? (1.0f / texH) : 0.0f;

    SpriteInstance instance = {};
    instance.translation = Vector4{
        position.x,
        position.y,
        params.scale.x * static_cast<f32>(sourceRect.width),
        params.scale.y * static_cast<f32>(sourceRect.height),
    };
    instance.sourceRect = Vector4{
        static_cast<f32>(sourceRect.x) * invW,
        static_cast<f32>(sourceRect.y) * invH,
        static_cast<f32>(sourceRect.width) * invW,
        static_cast<f32>(sourceRect.height) * invH,
    };
    instance.originRotationWaterLine = Vector4{
        params.originPivot.x,
        params.originPivot.y,
        params.rotation.get(),
        params.waterLineYPosition,
    };
    instance.color0 = params.color;
    instance.color1 = params.color1;
    instance.color2 = params.color;
    instance.reservedBlendFactor = Color{
        0,
        0,
        static_cast<u8>(std::clamp(static_cast<i32>(params.blendFactor * 255.0f), 0, 255)),
        static_cast<u8>(std::clamp(static_cast<i32>(params.blendFactorForWaterLine * 255.0f), 0, 255)),
    };

    instances_.push_back(std::move(instance));
    textures_.push_back(texture);
    layerDepths_.push_back(params.layerDepth);
}

void SpriteBatchImpl::sort(SpriteSortMode sortMode)
{
    POMDOG_ASSERT(instances_.size() == textures_.size());
    POMDOG_ASSERT(instances_.size() == layerDepths_.size());

    if (nextInstance_ >= instances_.size()) {
        return;
    }

    POMDOG_ASSERT(nextInstance_ < instances_.size());
    POMDOG_ASSERT(nextInstance_ < textures_.size());
    POMDOG_ASSERT(nextInstance_ < layerDepths_.size());

#if defined(__GLIBCXX__)
    // FIXME: Use index-based sorting instead of zip_view + stable_sort,
    // because libstdc++'s zip_view iterator only exposes input_iterator_tag
    // as its legacy category, which breaks std::stable_sort internally.
    //
    // gcc:
    // ```
    // /usr/include/c++/15.2.1/bits/stl_algo.h:1341:27: error: no matching function for call to ‘__rotate(std::ranges::zip_view<std::ranges::ref_view<std::vector<float, std::allocator<float> > >, ...
    // ```
    //
    // clang + libstdc++:
    // ```
    // /usr/lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/bits/stl_algo.h:1399:14: error: no matching function for call to '__rotate'ux-gnu/13/../../../../include/c++/13/bits/stl_algo.h:1399:14: error: no matching function for call to '__rotate'
    //  1399 |       return std::__rotate(__first, __middle, __last,(__first, __middle, __last,
    //       |              ^~~~~~~~~~~~~
    // ```
    //
    // This workaround results in additional memory allocations and copies during sorting,
    // but it is necessary for compatibility with GCC's libstdc++.
    constexpr bool useIndexBasedSorting = true;
#else
    constexpr bool useIndexBasedSorting = false;
#endif

    if constexpr (useIndexBasedSorting) {
        const auto sortCount = static_cast<u32>(instances_.size() - nextInstance_);
        std::vector<u32> indices(sortCount);
        std::iota(indices.begin(), indices.end(), nextInstance_);

        switch (sortMode) {
        case SpriteSortMode::Texture:
            std::stable_sort(indices.begin(), indices.end(), [&](u32 a, u32 b) {
                return textures_[a] < textures_[b];
            });
            break;
        case SpriteSortMode::BackToFront:
            std::stable_sort(indices.begin(), indices.end(), [&](u32 a, u32 b) {
                return layerDepths_[a] > layerDepths_[b];
            });
            break;
        case SpriteSortMode::FrontToBack:
            std::stable_sort(indices.begin(), indices.end(), [&](u32 a, u32 b) {
                return layerDepths_[a] < layerDepths_[b];
            });
            break;
        }

        // NOTE: Apply the permutation using temporary buffers.
        std::vector<SpriteInstance> sortedInstances(sortCount);
        std::vector<std::shared_ptr<gpu::Texture>> sortedTextures(sortCount);
        std::vector<f32> sortedDepths(sortCount);
        for (u32 i = 0; i < sortCount; ++i) {
            sortedInstances[i] = std::move(instances_[indices[i]]);
            sortedTextures[i] = std::move(textures_[indices[i]]);
            sortedDepths[i] = layerDepths_[indices[i]];
        }
        for (u32 i = 0; i < sortCount; ++i) {
            instances_[nextInstance_ + i] = std::move(sortedInstances[i]);
            textures_[nextInstance_ + i] = std::move(sortedTextures[i]);
            layerDepths_[nextInstance_ + i] = sortedDepths[i];
        }
    }
    else {
#if defined(__GLIBCXX__)
        POMDOG_UNREACHABLE("libstdc++'s zip_view does not meet the requirements for stable_sort");
#else
        auto zipped = std::views::zip(layerDepths_, textures_, instances_);
        auto droppedZipped = std::views::drop(zipped, nextInstance_);

        switch (sortMode) {
        case SpriteSortMode::Texture:
            std::ranges::stable_sort(droppedZipped, [](const auto& a, const auto& b) {
                return std::get<1>(a) < std::get<1>(b);
            });
            break;
        case SpriteSortMode::BackToFront:
            std::ranges::stable_sort(droppedZipped, [](const auto& a, const auto& b) {
                return std::get<0>(a) > std::get<0>(b);
            });
            break;
        case SpriteSortMode::FrontToBack:
            std::ranges::stable_sort(droppedZipped, [](const auto& a, const auto& b) {
                return std::get<0>(a) < std::get<0>(b);
            });
            break;
        }
#endif
    }
}

void SpriteBatchImpl::flush(
    const std::shared_ptr<gpu::CommandList>& commandList,
    const std::shared_ptr<SpritePipeline>& spritePipeline)
{
    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(spritePipeline);
    auto pipeline = static_cast<SpritePipelineImpl*>(spritePipeline.get());
    flushBatch(commandList, pipeline);
}

void SpriteBatchImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const Color& color)
{
    drawImpl(
        texture,
        position,
        sourceRect,
        SpriteBatchDrawParameters{
            .color = color,
            .originPivot = {0.5f, 0.5f},
        });
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
    drawImpl(
        texture,
        position,
        sourceRect,
        SpriteBatchDrawParameters{
            .color = color,
            .rotation = rotation,
            .originPivot = originPivot,
            .scale = {scale, scale},
        });
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
    drawImpl(
        texture,
        position,
        sourceRect,
        SpriteBatchDrawParameters{
            .color = color,
            .rotation = rotation,
            .originPivot = originPivot,
            .scale = scale,
        });
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
    drawImpl(
        texture,
        position,
        textureRegion.subrect,
        SpriteBatchDrawParameters{
            .color = color,
            .rotation = rotation,
            .originPivot = offset,
            .scale = {scale, scale},
        });
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
    drawImpl(
        texture,
        position,
        textureRegion.subrect,
        SpriteBatchDrawParameters{
            .color = color,
            .rotation = rotation,
            .originPivot = offset,
            .scale = scale,
        });
}

void SpriteBatchImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const SpriteBatchDrawParameters& params)
{
    drawImpl(texture, position, sourceRect, params);
}

void SpriteBatchImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const TextureRegion& textureRegion,
    const SpriteBatchDrawParameters& params)
{
    auto offset = computeSpriteOffset(textureRegion, params.originPivot);
    drawImpl(
        texture,
        position,
        textureRegion.subrect,
        SpriteBatchDrawParameters{
            .color = params.color,
            .color1 = params.color1,
            .blendFactor = params.blendFactor,
            .blendFactorForWaterLine = params.blendFactorForWaterLine,
            .waterLineYPosition = params.waterLineYPosition,
            .rotation = params.rotation,
            .originPivot = offset,
            .scale = params.scale,
            .layerDepth = params.layerDepth,
        });
}

u32 SpriteBatchImpl::getDrawCallCount() const noexcept
{
    return drawCallCount_;
}

class SpriteBatchSortedSingleTextureImpl final : public SpriteBatch {
private:
    static constexpr u32 DefaultBatchSize = 2048;
    static constexpr u32 MinBatchSize = 64;
    static constexpr u32 MaxBatchSize = 65536;

    std::vector<SpriteInstance> instances_;
    std::shared_ptr<gpu::VertexBuffer> instanceVertices_;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer_;
    std::shared_ptr<gpu::Texture> texture_;
    Vector2 inverseTextureSize_;
    u32 maxBatchSize_ = DefaultBatchSize;
    u32 drawCallCount_ = 0;
    u32 nextInstance_ = 0;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<u32> batchSize);

    void reset() override;

    void setTransform(const Matrix4x4& transformMatrix) override;

    void setTransform(
        const Matrix4x4& transformMatrix,
        f32 fontSmoothing,
        f32 fontWeight,
        const Color& outlineColor,
        f32 outlineWeight) override;

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

    void draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const SpriteBatchDrawParameters& params) override;

    void draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const SpriteBatchDrawParameters& params) override;

    void sort([[maybe_unused]] SpriteSortMode sortMode) override;

    void flush(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<SpritePipeline>& spritePipeline) override;

    void submit(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) override;

    [[nodiscard]] u32
    getDrawCallCount() const noexcept override;

private:
    void setTransformImpl(
        const Matrix4x4& transformMatrix,
        f32 fontSmoothing,
        f32 fontWeight,
        const Color& outlineColor,
        f32 outlineWeight);

    void drawImpl(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const SpriteBatchDrawParameters& params);
};

std::unique_ptr<Error>
SpriteBatchSortedSingleTextureImpl::initialize(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize)
{
    if (batchSize.has_value()) {
        if (*batchSize < MinBatchSize) {
            return errors::make("batchSize is too small (minimum: " + std::to_string(MinBatchSize) + ")");
        }
        if (*batchSize > MaxBatchSize) {
            return errors::make("batchSize is too large (maximum: " + std::to_string(MaxBatchSize) + ")");
        }
        maxBatchSize_ = *batchSize;
    }

    {
        if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
                maxBatchSize_,
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

    instances_.reserve(MinBatchSize);

    return nullptr;
}

void SpriteBatchSortedSingleTextureImpl::reset()
{
    texture_ = nullptr;
    instances_.clear();
    nextInstance_ = 0;
    drawCallCount_ = 0;
}

void SpriteBatchSortedSingleTextureImpl::setTransform(const Matrix4x4& transformMatrix)
{
    setTransformImpl(transformMatrix, 0.08f, 0.48f, Color::createTransparentBlack(), 0.28f);
}

void SpriteBatchSortedSingleTextureImpl::setTransform(
    const Matrix4x4& transformMatrix,
    f32 fontSmoothing,
    f32 fontWeight,
    const Color& outlineColor,
    f32 outlineWeight)
{
    setTransformImpl(transformMatrix, fontSmoothing, fontWeight, outlineColor, outlineWeight);
}

void SpriteBatchSortedSingleTextureImpl::setTransformImpl(
    const Matrix4x4& transformMatrix,
    f32 fontSmoothing,
    f32 fontWeight,
    const Color& outlineColor,
    f32 outlineWeight)
{
    POMDOG_ASSERT(constantBuffer_);

    SpriteBatchConstantBuffer constants;
    constants.viewProjection = math::transpose(transformMatrix);
    constants.distanceFieldParameters.x = fontSmoothing;
    constants.distanceFieldParameters.y = fontWeight;
    constants.distanceFieldParameters.z = outlineWeight;
    constants.distanceFieldParameters.w = 0.0f;
    constants.outlineColor = outlineColor.toVector4();

    constantBuffer_->setData(0, gpu::makeByteSpan(constants));
}

void SpriteBatchSortedSingleTextureImpl::drawImpl(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const SpriteBatchDrawParameters& params)
{
    POMDOG_ASSERT(texture != nullptr);
    POMDOG_ASSERT(texture->getWidth() > 0);
    POMDOG_ASSERT(texture->getHeight() > 0);
    POMDOG_ASSERT(sourceRect.width >= 0);
    POMDOG_ASSERT(sourceRect.height >= 0);

    if (texture == nullptr) [[unlikely]] {
        return;
    }

    if ((sourceRect.width == 0) || (sourceRect.height == 0)) [[unlikely]] {
        return;
    }

    if ((params.scale.x == 0.0f) || (params.scale.y == 0.0f)) [[unlikely]] {
        return;
    }

    if (texture_ == nullptr) {
        texture_ = texture;
        inverseTextureSize_ = Vector2{
            1.0f / static_cast<f32>(texture_->getWidth()),
            1.0f / static_cast<f32>(texture_->getHeight()),
        };
    }

    POMDOG_ASSERT(texture_ != nullptr);
    POMDOG_ASSERT(sourceRect.width > 0);
    POMDOG_ASSERT(sourceRect.height > 0);
    POMDOG_ASSERT(texture_->getWidth() > 0);
    POMDOG_ASSERT(texture_->getHeight() > 0);

    SpriteInstance instance = {};
    instance.translation = Vector4{
        position.x,
        position.y,
        params.scale.x * static_cast<f32>(sourceRect.width),
        params.scale.y * static_cast<f32>(sourceRect.height),
    };
    instance.sourceRect = Vector4{
        static_cast<f32>(sourceRect.x) * inverseTextureSize_.x,
        static_cast<f32>(sourceRect.y) * inverseTextureSize_.y,
        static_cast<f32>(sourceRect.width) * inverseTextureSize_.x,
        static_cast<f32>(sourceRect.height) * inverseTextureSize_.y,
    };
    instance.originRotationWaterLine = Vector4{
        params.originPivot.x,
        params.originPivot.y,
        params.rotation.get(),
        params.waterLineYPosition,
    };
    instance.color0 = params.color;
    instance.color1 = params.color1;
    instance.color2 = params.color;
    instance.reservedBlendFactor = Color{
        0,
        0,
        static_cast<u8>(std::clamp(static_cast<i32>(params.blendFactor * 255.0f), 0, 255)),
        static_cast<u8>(std::clamp(static_cast<i32>(params.blendFactorForWaterLine * 255.0f), 0, 255)),
    };

    instances_.push_back(std::move(instance));
}

void SpriteBatchSortedSingleTextureImpl::sort([[maybe_unused]] SpriteSortMode sortMode)
{
    // NOTE: Nothing to do for single-texture batches.
}

void SpriteBatchSortedSingleTextureImpl::flush(
    const std::shared_ptr<gpu::CommandList>& commandList,
    const std::shared_ptr<SpritePipeline>& spritePipeline)
{
    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(spritePipeline);

    const auto maxInstanceCount = std::min(maxBatchSize_, static_cast<u32>(instances_.size()));
    if (nextInstance_ >= maxInstanceCount) {
        return;
    }

    if (texture_ == nullptr) {
        return;
    }

    const u32 instanceCountPerDraw = maxInstanceCount - nextInstance_;

    auto pipeline = static_cast<SpritePipelineImpl*>(spritePipeline.get());
    POMDOG_ASSERT(pipeline);

    commandList->setTexture(0, texture_);
    commandList->setSamplerState(0, pipeline->sampler_);

    commandList->setPipelineState(pipeline->pipelineState_);
    commandList->setConstantBuffer(0, constantBuffer_);
    commandList->setVertexBuffer(0, pipeline->planeVertices_);
    commandList->setVertexBuffer(1, instanceVertices_);
    commandList->setIndexBuffer(pipeline->planeIndices_);

    commandList->drawIndexedInstanced(
        pipeline->planeIndices_->getIndexCount(),
        instanceCountPerDraw,
        0,
        nextInstance_);

    ++drawCallCount_;
    nextInstance_ = maxInstanceCount;
}

void SpriteBatchSortedSingleTextureImpl::submit(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    // NOTE: Grow the vertex buffer if needed (takes effect on next frame).
    [&] {
        const auto instanceCount = static_cast<u32>(instances_.size());
        if (instanceCount <= maxBatchSize_) {
            return;
        }

        if (graphicsDevice == nullptr) {
            return;
        }

        constexpr u32 divisor = 2048;
        const auto requiredSize = ((instanceCount + divisor - 1) / divisor) * divisor;

        if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
                requiredSize,
                sizeof(SpriteInstance),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            // NOTE: Failed to resize; sprites beyond maxBatchSize_ are dropped this frame.
        }
        else {
            instanceVertices_ = std::move(buffer);
            maxBatchSize_ = requiredSize;
        }
    }();

    // NOTE: Upload instance data to GPU.
    if (!instances_.empty()) {
        const auto instanceCount = std::min(maxBatchSize_, static_cast<u32>(instances_.size()));
        instanceVertices_->setData(
            0,
            instances_.data(),
            instanceCount,
            sizeof(SpriteInstance));
    }

    // NOTE: Shrink over-allocated CPU vectors.
    if (instances_.capacity() > maxBatchSize_) {
        instances_.resize(maxBatchSize_);
        instances_.shrink_to_fit();
    }
}

void SpriteBatchSortedSingleTextureImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const Color& color)
{
    drawImpl(
        texture,
        position,
        sourceRect,
        SpriteBatchDrawParameters{
            .color = color,
            .originPivot = {0.5f, 0.5f},
        });
}

void SpriteBatchSortedSingleTextureImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    f32 scale)
{
    drawImpl(
        texture,
        position,
        sourceRect,
        SpriteBatchDrawParameters{
            .color = color,
            .rotation = rotation,
            .originPivot = originPivot,
            .scale = {scale, scale},
        });
}

void SpriteBatchSortedSingleTextureImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    const Vector2& scale)
{
    drawImpl(
        texture,
        position,
        sourceRect,
        SpriteBatchDrawParameters{
            .color = color,
            .rotation = rotation,
            .originPivot = originPivot,
            .scale = scale,
        });
}

void SpriteBatchSortedSingleTextureImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const TextureRegion& textureRegion,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    f32 scale)
{
    auto offset = computeSpriteOffset(textureRegion, originPivot);
    drawImpl(
        texture,
        position,
        textureRegion.subrect,
        SpriteBatchDrawParameters{
            .color = color,
            .rotation = rotation,
            .originPivot = offset,
            .scale = {scale, scale},
        });
}

void SpriteBatchSortedSingleTextureImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const TextureRegion& textureRegion,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    const Vector2& scale)
{
    auto offset = computeSpriteOffset(textureRegion, originPivot);
    drawImpl(
        texture,
        position,
        textureRegion.subrect,
        SpriteBatchDrawParameters{
            .color = color,
            .rotation = rotation,
            .originPivot = offset,
            .scale = scale,
        });
}

void SpriteBatchSortedSingleTextureImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const Rect2D& sourceRect,
    const SpriteBatchDrawParameters& params)
{
    drawImpl(texture, position, sourceRect, params);
}

void SpriteBatchSortedSingleTextureImpl::draw(
    const std::shared_ptr<gpu::Texture>& texture,
    const Vector2& position,
    const TextureRegion& textureRegion,
    const SpriteBatchDrawParameters& params)
{
    auto offset = computeSpriteOffset(textureRegion, params.originPivot);
    drawImpl(
        texture,
        position,
        textureRegion.subrect,
        SpriteBatchDrawParameters{
            .color = params.color,
            .color1 = params.color1,
            .blendFactor = params.blendFactor,
            .blendFactorForWaterLine = params.blendFactorForWaterLine,
            .waterLineYPosition = params.waterLineYPosition,
            .rotation = params.rotation,
            .originPivot = offset,
            .scale = params.scale,
            .layerDepth = params.layerDepth,
        });
}

u32 SpriteBatchSortedSingleTextureImpl::getDrawCallCount() const noexcept
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
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize) noexcept
{
    auto spriteBatch = std::make_shared<SpriteBatchImpl>();
    if (auto err = spriteBatch->initialize(graphicsDevice, batchSize); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(spriteBatch), nullptr);
}

std::tuple<std::shared_ptr<SpriteBatch>, std::unique_ptr<Error>>
createSpriteBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize,
    SpriteBatchOptimizationKind optimization) noexcept
{
    if (optimization == SpriteBatchOptimizationKind::NotSpecified) {
        return createSpriteBatch(graphicsDevice, batchSize);
    }

    auto spriteBatch = std::make_shared<SpriteBatchSortedSingleTextureImpl>();
    if (auto err = spriteBatch->initialize(graphicsDevice, batchSize); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(spriteBatch), nullptr);
}

} // namespace pomdog
