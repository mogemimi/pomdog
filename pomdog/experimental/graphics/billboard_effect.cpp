// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/billboard_effect.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/shader_loader.h"
#include "pomdog/gpu/blend_desc.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/depth_stencil_desc.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/input_layout_helper.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/rasterizer_desc.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_pipeline_stage.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/math/color.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

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

} // namespace

class BillboardBatchBuffer::Impl final {
public:
    std::vector<BillboardInfo> instances;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer;
};

BillboardBatchBuffer::BillboardBatchBuffer() = default;

BillboardBatchBuffer::~BillboardBatchBuffer() = default;

std::unique_ptr<Error>
BillboardBatchBuffer::initialize(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    u32 capacity)
{
    impl = std::make_unique<Impl>();
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(graphicsDevice);
    impl->instances.reserve(capacity);
    if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
            capacity,
            sizeof(BillboardInfo),
            gpu::BufferUsage::Dynamic);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create vertex buffer");
    }
    else {
        impl->vertexBuffer = std::move(buffer);
    }
    return nullptr;
}

void BillboardBatchBuffer::reset()
{
    impl->instances.clear();
}

void BillboardBatchBuffer::addBillboard(
    const Vector3& position,
    const Color& color,
    f32 scale)
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
    const Radian<f32>& rotationZ,
    const Vector2& originPivot,
    f32 scale)
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
    const Radian<f32>& rotationZ,
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
    info.PositionRotation.w = rotationZ.get();
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
        static_cast<u32>(impl->instances.size()),
        sizeof(BillboardInfo));
}

const std::shared_ptr<gpu::VertexBuffer>& BillboardBatchBuffer::getVertexBuffer() const
{
    POMDOG_ASSERT(impl);
    return impl->vertexBuffer;
}

u32 BillboardBatchBuffer::getSize() const noexcept
{
    POMDOG_ASSERT(impl);
    return static_cast<u32>(impl->instances.size());
}

u32 BillboardBatchBuffer::getCapacity() const noexcept
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->vertexBuffer);
    return impl->vertexBuffer->getVertexCount();
}

class BillboardBatchEffect::Impl final {
public:
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer;
    std::shared_ptr<gpu::IndexBuffer> indexBuffer;
    std::shared_ptr<gpu::PipelineState> pipelineState;
};

BillboardBatchEffect::BillboardBatchEffect() = default;

std::unique_ptr<Error>
BillboardBatchEffect::initialize(
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
        std::nullopt);
}

std::unique_ptr<Error>
BillboardBatchEffect::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::BlendDesc>&& blendDesc,
    std::optional<gpu::DepthStencilDesc>&& depthStencilDesc,
    std::optional<gpu::RasterizerDesc>&& rasterizerDesc,
    std::optional<gpu::PixelFormat>&& renderTargetViewFormat,
    std::optional<gpu::PixelFormat>&& depthStencilViewFormat)
{
    impl = std::make_unique<Impl>();
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(graphicsDevice);

    auto presentationParameters = graphicsDevice->getPresentationParameters();

    if (!blendDesc) {
        blendDesc = gpu::BlendDesc::createNonPremultiplied();
    }
    if (!depthStencilDesc) {
        depthStencilDesc = gpu::DepthStencilDesc::createDefault();
    }
    if (!rasterizerDesc) {
        rasterizerDesc = gpu::RasterizerDesc::createCullNone();
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
        if (auto [buffer, err] = graphicsDevice->createVertexBuffer(
                vertices.data(),
                static_cast<u32>(vertices.size()),
                sizeof(PositionTextureCoord),
                gpu::BufferUsage::Immutable);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
        else {
            impl->vertexBuffer = std::move(buffer);
        }
    }
    {
        std::array<std::uint16_t, 6> const indices = {{0, 1, 2, 2, 3, 0}};

        // NOTE: Create index buffer
        if (auto [indexBuf, indexBufErr] = graphicsDevice->createIndexBuffer(
                gpu::IndexFormat::UInt16,
                indices.data(),
                static_cast<u32>(indices.size()),
                gpu::BufferUsage::Immutable);
            indexBufErr != nullptr) {
            return errors::wrap(std::move(indexBufErr), "failed to create index buffer");
        }
        else {
            impl->indexBuffer = std::move(indexBuf);
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
                               .addFloat4();

        auto [vertexShader, vsErr] = loadShaderAutomagically(
            fs, graphicsDevice,
            gpu::ShaderPipelineStage::VertexShader,
            "/assets/shaders", "billboard_batch_vs", "billboard_batch_vs");
        if (vsErr != nullptr) {
            return errors::wrap(std::move(vsErr), "failed to load vertex shader");
        }

        auto [pixelShader, psErr] = loadShaderAutomagically(
            fs, graphicsDevice,
            gpu::ShaderPipelineStage::PixelShader,
            "/assets/shaders", "billboard_batch_ps", "billboard_batch_ps");
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
        pipelineStateBuilder.setDepthStencilState(*depthStencilDesc);
        pipelineStateBuilder.setRasterizerState(*rasterizerDesc);

        auto [pipeline, pipelineErr] = pipelineStateBuilder.build();
        if (pipelineErr != nullptr) {
            return errors::wrap(std::move(pipelineErr), "failed to create pipeline state");
        }
        impl->pipelineState = std::move(pipeline);
    }

    return nullptr;
}

BillboardBatchEffect::~BillboardBatchEffect() = default;

void BillboardBatchEffect::draw(
    const std::shared_ptr<gpu::CommandList>& commandList,
    const std::shared_ptr<gpu::Texture2D>& texture,
    const std::shared_ptr<gpu::SamplerState>& sampler,
    const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer,
    u32 constantBufferOffset,
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
