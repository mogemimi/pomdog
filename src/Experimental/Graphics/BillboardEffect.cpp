// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Graphics/BillboardEffect.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/RasterizerDescription.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/ShaderPipelineStage.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <array>
#include <cstring>

namespace Pomdog {
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
#include "Shaders/GLSL.Embedded/BillboardBatch_PS.inc.hpp"
#include "Shaders/GLSL.Embedded/BillboardBatch_VS.inc.hpp"
#include "Shaders/HLSL.Embedded/BillboardBatch_PS.inc.hpp"
#include "Shaders/HLSL.Embedded/BillboardBatch_VS.inc.hpp"
#include "Shaders/Metal.Embedded/BillboardBatch.inc.hpp"

} // namespace

class BillboardBatchBuffer::Impl final {
public:
    std::vector<BillboardInfo> instances;
    std::shared_ptr<VertexBuffer> vertexBuffer;
};

BillboardBatchBuffer::BillboardBatchBuffer(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    int capacity)
    : impl(std::make_unique<Impl>())
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(graphicsDevice);
    const auto maxBatchSize = static_cast<std::size_t>(capacity);
    impl->instances.reserve(maxBatchSize);
    impl->vertexBuffer = std::get<0>(graphicsDevice->CreateVertexBuffer(
        maxBatchSize,
        sizeof(BillboardInfo),
        BufferUsage::Dynamic));
}

BillboardBatchBuffer::~BillboardBatchBuffer() = default;

void BillboardBatchBuffer::Reset()
{
    impl->instances.clear();
}

void BillboardBatchBuffer::AddBillboard(
    const Vector3& position,
    const Color& color,
    float scale)
{
    AddBillboard(
        position,
        Vector2{0.0f, 0.0f},
        Vector2{1.0f, 1.0f},
        color,
        0.0f,
        Vector2{0.5f, 0.5f},
        Vector2{scale, scale});
}

void BillboardBatchBuffer::AddBillboard(
    const Vector3& position,
    const Color& color,
    const Radian<float>& rotationZ,
    const Vector2& originPivot,
    float scale)
{
    AddBillboard(
        position,
        Vector2{0.0f, 0.0f},
        Vector2{1.0f, 1.0f},
        color,
        rotationZ,
        originPivot,
        Vector2{scale, scale});
}

void BillboardBatchBuffer::AddBillboard(
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
    POMDOG_ASSERT(impl->vertexBuffer->GetVertexCount() == impl->instances.capacity());

    if (impl->instances.size() >= impl->instances.capacity()) {
        return;
    }

    if ((scale.X == 0.0f) || (scale.Y == 0.0f)) {
        return;
    }

    BillboardInfo info;
    info.PositionRotation.X = position.X;
    info.PositionRotation.Y = position.Y;
    info.PositionRotation.Z = position.Z;
    info.PositionRotation.W = rotationZ.value;
    info.TextureCoordRect.X = textureCoord.X;
    info.TextureCoordRect.Y = textureCoord.Y;
    info.TextureCoordRect.Z = textureSize.X;
    info.TextureCoordRect.W = textureSize.Y;
    info.OriginPivotScale.X = originPivot.X;
    info.OriginPivotScale.Y = originPivot.Y;
    info.OriginPivotScale.Z = scale.X;
    info.OriginPivotScale.W = scale.Y;
    info.Color = color.ToVector4();

    impl->instances.push_back(std::move(info));
}

void BillboardBatchBuffer::FetchBuffer()
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->vertexBuffer);
    POMDOG_ASSERT(impl->instances.size() <= impl->vertexBuffer->GetVertexCount());

    if (impl->instances.empty()) {
        return;
    }

    impl->vertexBuffer->SetData(
        0,
        impl->instances.data(),
        impl->instances.size(),
        sizeof(BillboardInfo));
}

const std::shared_ptr<VertexBuffer>& BillboardBatchBuffer::GetVertexBuffer() const
{
    POMDOG_ASSERT(impl);
    return impl->vertexBuffer;
}

int BillboardBatchBuffer::GetSize() const noexcept
{
    POMDOG_ASSERT(impl);
    return static_cast<int>(impl->instances.size());
}

int BillboardBatchBuffer::GetCapacity() const noexcept
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->vertexBuffer);
    return static_cast<int>(impl->vertexBuffer->GetVertexCount());
}

class BillboardBatchEffect::Impl final {
public:
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<PipelineState> pipelineState;
};

BillboardBatchEffect::BillboardBatchEffect(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
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
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    std::optional<BlendDescription>&& blendDesc,
    std::optional<DepthStencilDescription>&& depthStencilDesc,
    std::optional<RasterizerDescription>&& rasterizerDesc,
    std::optional<SurfaceFormat>&& renderTargetViewFormat,
    std::optional<SurfaceFormat>&& depthStencilViewFormat,
    AssetManager& assets)
    : impl(std::make_unique<Impl>())
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(graphicsDevice);

    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    if (!blendDesc) {
        blendDesc = BlendDescription::CreateNonPremultiplied();
    }
    if (!depthStencilDesc) {
        depthStencilDesc = DepthStencilDescription::CreateDefault();
    }
    if (!rasterizerDesc) {
        rasterizerDesc = RasterizerDescription::CreateCullNone();
    }
    if (!renderTargetViewFormat) {
        renderTargetViewFormat = presentationParameters.BackBufferFormat;
    }
    if (!depthStencilViewFormat) {
        depthStencilViewFormat = presentationParameters.DepthStencilFormat;
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
        impl->vertexBuffer = std::get<0>(graphicsDevice->CreateVertexBuffer(
            vertices.data(),
            vertices.size(),
            sizeof(PositionTextureCoord),
            BufferUsage::Immutable));
    }
    {
        std::array<std::uint16_t, 6> const indices = {{
            0, 1, 2,
            2, 3, 0
        }};

        // NOTE: Create index buffer
        impl->indexBuffer = std::get<0>(graphicsDevice->CreateIndexBuffer(
            IndexElementSize::SixteenBits,
            indices.data(),
            indices.size(),
            BufferUsage::Immutable));
    }
    {
        auto inputLayout = InputLayoutHelper{}
            .AddInputSlot()
            .Float4()
            .AddInputSlot(InputClassification::InputPerInstance, 1)
            .Float4()
            .Float4()
            .Float4()
            .Float4();

        auto vertexShaderBuilder = assets.CreateBuilder<Shader>(ShaderPipelineStage::VertexShader)
            .SetGLSL(Builtin_GLSL_BillboardBatch_VS, std::strlen(Builtin_GLSL_BillboardBatch_VS))
            .SetHLSLPrecompiled(BuiltinHLSL_BillboardBatch_VS, sizeof(BuiltinHLSL_BillboardBatch_VS))
            .SetMetal(Builtin_Metal_BillboardBatch, sizeof(Builtin_Metal_BillboardBatch), "BillboardBatchVS");

        auto pixelShaderBuilder = assets.CreateBuilder<Shader>(ShaderPipelineStage::PixelShader)
            .SetGLSL(Builtin_GLSL_BillboardBatch_PS, std::strlen(Builtin_GLSL_BillboardBatch_PS))
            .SetHLSLPrecompiled(BuiltinHLSL_BillboardBatch_PS, sizeof(BuiltinHLSL_BillboardBatch_PS))
            .SetMetal(Builtin_Metal_BillboardBatch, sizeof(Builtin_Metal_BillboardBatch), "BillboardBatchPS");

        auto [vertexShader, vertexShaderErr] = vertexShaderBuilder.Build();
        if (vertexShaderErr != nullptr) {
            // FIXME: error handling
        }

        auto [pixelShader, pixelShaderErr] = pixelShaderBuilder.Build();
        if (pixelShaderErr != nullptr) {
            // FIXME: error handling
        }

        std::unique_ptr<Error> pipelineStateErr;
        std::tie(impl->pipelineState, pipelineStateErr) = assets.CreateBuilder<PipelineState>()
            .SetRenderTargetViewFormat(*renderTargetViewFormat)
            .SetDepthStencilViewFormat(*depthStencilViewFormat)
            .SetVertexShader(std::move(vertexShader))
            .SetPixelShader(std::move(pixelShader))
            .SetInputLayout(inputLayout.CreateInputLayout())
            .SetPrimitiveTopology(PrimitiveTopology::TriangleList)
            .SetBlendState(*blendDesc)
            .SetDepthStencilState(*depthStencilDesc)
            .SetRasterizerState(*rasterizerDesc)
            .SetConstantBufferBindSlot("WorldConstants", 0)
            .Build();
        if (pipelineStateErr != nullptr) {
            // FIXME: error handling
        }
    }
}

BillboardBatchEffect::~BillboardBatchEffect() = default;

void BillboardBatchEffect::Draw(
    const std::shared_ptr<GraphicsCommandList>& commandList,
    const std::shared_ptr<Texture2D>& texture,
    const std::shared_ptr<SamplerState>& sampler,
    const std::shared_ptr<ConstantBuffer>& constantBuffer,
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
    POMDOG_ASSERT(constantBufferOffset >= 0);

    if (billboardInstances.GetSize() <= 0) {
        return;
    }

    commandList->SetTexture(0, texture);
    commandList->SetSamplerState(0, sampler);

    commandList->SetPipelineState(impl->pipelineState);
    commandList->SetConstantBuffer(0, constantBuffer, constantBufferOffset);
    commandList->SetVertexBuffer(0, impl->vertexBuffer);
    commandList->SetVertexBuffer(1, billboardInstances.GetVertexBuffer());

    commandList->DrawIndexedInstanced(
        impl->indexBuffer,
        impl->indexBuffer->GetIndexCount(),
        billboardInstances.GetSize(),
        0,
        0);
}

} // namespace Pomdog
