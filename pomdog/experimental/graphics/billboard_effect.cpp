// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/billboard_effect.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/asset_builders/shader_builder.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/gpu/blend_descriptor.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/depth_stencil_descriptor.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/input_layout_helper.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/rasterizer_descriptor.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_pipeline_stage.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/math/color.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

struct alignas(16) BillboardInfo final {
    // {xyz_} = position.xyz
    // {___w} = rotation.z
    Vector4 PositionRotation;

    // {xy__} = textureCoord.xy
    // {__zw} = textureSize.xy
    Vector4 TextureCoordRect;

    // {xy__} = originPivot.xy
    // {__zw} = scale.xy
    Vector4 OriginPivotScale;

    // {xyz_} = color.rgb
    // {___w} = color.a
    Vector4 Color;
};

// Built-in shaders
#include "shaders/glsl.embedded/billboard_batch_ps.inc.h"
#include "shaders/glsl.embedded/billboard_batch_vs.inc.h"
#include "shaders/hlsl.embedded/billboard_batch_ps.inc.h"
#include "shaders/hlsl.embedded/billboard_batch_vs.inc.h"
#include "shaders/metal.embedded/billboard_batch.inc.h"

} // namespace

class BillboardBatchBuffer::Impl final {
public:
    std::vector<BillboardInfo> instances;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer;
};

BillboardBatchBuffer::BillboardBatchBuffer(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    int capacity)
    : impl(std::make_unique<Impl>())
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(graphicsDevice);
    const auto maxBatchSize = static_cast<std::size_t>(capacity);
    impl->instances.reserve(maxBatchSize);
    impl->vertexBuffer = std::get<0>(graphicsDevice->createVertexBuffer(
        maxBatchSize,
        sizeof(BillboardInfo),
        gpu::BufferUsage::Dynamic));
}

BillboardBatchBuffer::~BillboardBatchBuffer() = default;

void BillboardBatchBuffer::reset()
{
    impl->instances.clear();
}

void BillboardBatchBuffer::addBillboard(
    const Vector3& position,
    const Color& color,
    float scale)
{
    addBillboard(
        position,
        Vector2{0.0f, 0.0f},
        Vector2{1.0f, 1.0f},
        color,
        0.0f,
        Vector2{0.5f, 0.5f},
        Vector2{scale, scale});
}

void BillboardBatchBuffer::addBillboard(
    const Vector3& position,
    const Color& color,
    const Radian<float>& rotationZ,
    const Vector2& originPivot,
    float scale)
{
    addBillboard(
        position,
        Vector2{0.0f, 0.0f},
        Vector2{1.0f, 1.0f},
        color,
        rotationZ,
        originPivot,
        Vector2{scale, scale});
}

void BillboardBatchBuffer::addBillboard(
    const Vector3& position,
    const Vector2& textureCoord,
    const Vector2& textureSize,
    const Color& color,
    const Radian<float>& rotationZ,
    const Vector2& originPivot,
    const Vector2& scale)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->vertexBuffer);
    POMDOG_ASSERT(impl->vertexBuffer->getVertexCount() == impl->instances.capacity());

    if (impl->instances.size() >= impl->instances.capacity()) {
        return;
    }

    if ((scale.x == 0.0f) || (scale.y == 0.0f)) {
        return;
    }

    BillboardInfo info;
    info.PositionRotation.x = position.x;
    info.PositionRotation.y = position.y;
    info.PositionRotation.z = position.z;
    info.PositionRotation.w = rotationZ.value;
    info.TextureCoordRect.x = textureCoord.x;
    info.TextureCoordRect.y = textureCoord.y;
    info.TextureCoordRect.z = textureSize.x;
    info.TextureCoordRect.w = textureSize.y;
    info.OriginPivotScale.x = originPivot.x;
    info.OriginPivotScale.y = originPivot.y;
    info.OriginPivotScale.z = scale.x;
    info.OriginPivotScale.w = scale.y;
    info.Color = color.toVector4();

    impl->instances.push_back(std::move(info));
}

void BillboardBatchBuffer::fetchBuffer()
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->vertexBuffer);
    POMDOG_ASSERT(impl->instances.size() <= impl->vertexBuffer->getVertexCount());

    if (impl->instances.empty()) {
        return;
    }

    impl->vertexBuffer->setData(
        0,
        impl->instances.data(),
        impl->instances.size(),
        sizeof(BillboardInfo));
}

const std::shared_ptr<gpu::VertexBuffer>& BillboardBatchBuffer::getVertexBuffer() const
{
    POMDOG_ASSERT(impl);
    return impl->vertexBuffer;
}

int BillboardBatchBuffer::getSize() const noexcept
{
    POMDOG_ASSERT(impl);
    return static_cast<int>(impl->instances.size());
}

int BillboardBatchBuffer::getCapacity() const noexcept
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->vertexBuffer);
    return static_cast<int>(impl->vertexBuffer->getVertexCount());
}

class BillboardBatchEffect::Impl final {
public:
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer;
    std::shared_ptr<gpu::IndexBuffer> indexBuffer;
    std::shared_ptr<gpu::PipelineState> pipelineState;
};

BillboardBatchEffect::BillboardBatchEffect(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
    : BillboardBatchEffect(
          graphicsDevice,
          std::nullopt,
          std::nullopt,
          std::nullopt,
          std::nullopt,
          std::nullopt,
          assets)
{
}

BillboardBatchEffect::BillboardBatchEffect(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::BlendDescriptor>&& blendDesc,
    std::optional<gpu::DepthStencilDescriptor>&& depthStencilDesc,
    std::optional<gpu::RasterizerDescriptor>&& rasterizerDesc,
    std::optional<PixelFormat>&& renderTargetViewFormat,
    std::optional<PixelFormat>&& depthStencilViewFormat,
    AssetManager& assets)
    : impl(std::make_unique<Impl>())
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(graphicsDevice);

    auto presentationParameters = graphicsDevice->getPresentationParameters();

    if (!blendDesc) {
        blendDesc = gpu::BlendDescriptor::createNonPremultiplied();
    }
    if (!depthStencilDesc) {
        depthStencilDesc = gpu::DepthStencilDescriptor::createDefault();
    }
    if (!rasterizerDesc) {
        rasterizerDesc = gpu::RasterizerDescriptor::createCullNone();
    }
    if (!renderTargetViewFormat) {
        renderTargetViewFormat = presentationParameters.backBufferFormat;
    }
    if (!depthStencilViewFormat) {
        depthStencilViewFormat = presentationParameters.depthStencilFormat;
    }

    POMDOG_ASSERT(blendDesc);
    POMDOG_ASSERT(depthStencilDesc);
    POMDOG_ASSERT(rasterizerDesc);
    POMDOG_ASSERT(renderTargetViewFormat);
    POMDOG_ASSERT(depthStencilViewFormat);

    {
        using PositionTextureCoord = Vector4;

        std::array<PositionTextureCoord, 4> const vertices = {{
            Vector4{0.0f, 0.0f, 0.0f, 1.0f},
            Vector4{0.0f, 1.0f, 0.0f, 0.0f},
            Vector4{1.0f, 1.0f, 1.0f, 0.0f},
            Vector4{1.0f, 0.0f, 1.0f, 1.0f},
        }};

        // NOTE: Create vertex buffer
        impl->vertexBuffer = std::get<0>(graphicsDevice->createVertexBuffer(
            vertices.data(),
            vertices.size(),
            sizeof(PositionTextureCoord),
            gpu::BufferUsage::Immutable));
    }
    {
        std::array<std::uint16_t, 6> const indices = {{0, 1, 2, 2, 3, 0}};

        // NOTE: Create index buffer
        impl->indexBuffer = std::get<0>(graphicsDevice->createIndexBuffer(
            gpu::IndexFormat::UInt16,
            indices.data(),
            indices.size(),
            gpu::BufferUsage::Immutable));
    }
    {
        auto inputLayout = gpu::InputLayoutHelper{}
                               .addInputSlot()
                               .addFloat4()
                               .addInputSlot(gpu::InputClassification::InputPerInstance, 1)
                               .addFloat4()
                               .addFloat4()
                               .addFloat4()
                               .addFloat4();

        auto vertexShaderBuilder = assets.createBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader)
                                       .setGLSL(Builtin_GLSL_BillboardBatch_VS, std::strlen(Builtin_GLSL_BillboardBatch_VS))
                                       .setHLSLPrecompiled(BuiltinHLSL_BillboardBatch_VS, sizeof(BuiltinHLSL_BillboardBatch_VS))
                                       .setMetal(Builtin_Metal_BillboardBatch, sizeof(Builtin_Metal_BillboardBatch), "BillboardBatchVS");

        auto pixelShaderBuilder = assets.createBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader)
                                      .setGLSL(Builtin_GLSL_BillboardBatch_PS, std::strlen(Builtin_GLSL_BillboardBatch_PS))
                                      .setHLSLPrecompiled(BuiltinHLSL_BillboardBatch_PS, sizeof(BuiltinHLSL_BillboardBatch_PS))
                                      .setMetal(Builtin_Metal_BillboardBatch, sizeof(Builtin_Metal_BillboardBatch), "BillboardBatchPS");

        auto [vertexShader, vertexShaderErr] = vertexShaderBuilder.build();
        if (vertexShaderErr != nullptr) {
            // FIXME: error handling
        }

        auto [pixelShader, pixelShaderErr] = pixelShaderBuilder.build();
        if (pixelShaderErr != nullptr) {
            // FIXME: error handling
        }

        std::unique_ptr<Error> pipelineStateErr;
        std::tie(impl->pipelineState, pipelineStateErr) = assets.createBuilder<gpu::PipelineState>()
                                                              .setRenderTargetViewFormat(*renderTargetViewFormat)
                                                              .setDepthStencilViewFormat(*depthStencilViewFormat)
                                                              .setVertexShader(std::move(vertexShader))
                                                              .setPixelShader(std::move(pixelShader))
                                                              .setInputLayout(inputLayout.createInputLayout())
                                                              .setPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
                                                              .setBlendState(*blendDesc)
                                                              .setDepthStencilState(*depthStencilDesc)
                                                              .setRasterizerState(*rasterizerDesc)
                                                              .setConstantBufferBindSlot("WorldConstants", 0)
                                                              .build();
        if (pipelineStateErr != nullptr) {
            // FIXME: error handling
        }
    }
}

BillboardBatchEffect::~BillboardBatchEffect() = default;

void BillboardBatchEffect::draw(
    const std::shared_ptr<gpu::CommandList>& commandList,
    const std::shared_ptr<gpu::Texture2D>& texture,
    const std::shared_ptr<gpu::SamplerState>& sampler,
    const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer,
    std::size_t constantBufferOffset,
    const BillboardBatchBuffer& billboardInstances)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->vertexBuffer);
    POMDOG_ASSERT(impl->indexBuffer);
    POMDOG_ASSERT(impl->pipelineState);
    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(sampler);
    POMDOG_ASSERT(constantBuffer);
    static_assert(std::is_unsigned_v<decltype(constantBufferOffset)>, "constantBufferOffset >= 0");

    if (billboardInstances.getSize() <= 0) {
        return;
    }

    commandList->setTexture(0, texture);
    commandList->setSamplerState(0, sampler);

    commandList->setPipelineState(impl->pipelineState);
    commandList->setConstantBuffer(0, constantBuffer, constantBufferOffset);
    commandList->setVertexBuffer(0, impl->vertexBuffer);
    commandList->setVertexBuffer(1, billboardInstances.getVertexBuffer());
    commandList->setIndexBuffer(impl->indexBuffer);

    commandList->drawIndexedInstanced(
        impl->indexBuffer->getIndexCount(),
        billboardInstances.getSize(),
        0,
        0);
}

} // namespace pomdog
