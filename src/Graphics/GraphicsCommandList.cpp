// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeGraphicsCommandList.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Graphics/ClearOptions.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#include "Pomdog/Graphics/VertexBufferBinding.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {

GraphicsCommandList::GraphicsCommandList(GraphicsDevice & graphicsDevice)
{
    auto nativeDevice = graphicsDevice.GetNativeGraphicsDevice();
    POMDOG_ASSERT(nativeDevice != nullptr);

    nativeCommandList = nativeDevice->CreateGraphicsCommandList();
}

GraphicsCommandList::GraphicsCommandList(const std::shared_ptr<GraphicsDevice>& graphicsDevice)
    : GraphicsCommandList(*graphicsDevice)
{}

GraphicsCommandList::~GraphicsCommandList() = default;

void GraphicsCommandList::Close()
{
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->Close();
}

void GraphicsCommandList::Reset()
{
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->Reset();
}

std::size_t GraphicsCommandList::GetCount() const noexcept
{
    POMDOG_ASSERT(nativeCommandList);
    return nativeCommandList->GetCount();
}

void GraphicsCommandList::Clear(const Color& color)
{
    ///@todo badcode
    ClearOptions options = ClearOptions::RenderTarget
        | ClearOptions::DepthBuffer
        | ClearOptions::Stencil;

    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->Clear(options, color, 1.f, 0);
}

void GraphicsCommandList::Clear(ClearOptions options, const Color& color, float depth, std::uint8_t stencil)
{
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->Clear(options, color, depth, stencil);
}

void GraphicsCommandList::Draw(std::size_t vertexCount)
{
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->Draw(vertexCount);
}

void GraphicsCommandList::DrawIndexed(
    const std::shared_ptr<IndexBuffer>& indexBuffer,
    std::size_t indexCount)
{
    POMDOG_ASSERT(indexBuffer);
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->SetIndexBuffer(indexBuffer);
    nativeCommandList->DrawIndexed(indexCount);
}

void GraphicsCommandList::DrawInstanced(
    std::size_t vertexCount,
    std::size_t instanceCount)
{
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->DrawInstanced(vertexCount, instanceCount);
}

void GraphicsCommandList::DrawIndexedInstanced(
    const std::shared_ptr<IndexBuffer>& indexBuffer,
    std::size_t indexCount,
    std::size_t instanceCount)
{
    POMDOG_ASSERT(indexBuffer);
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->SetIndexBuffer(indexBuffer);
    nativeCommandList->DrawIndexedInstanced(indexCount, instanceCount);
}

void GraphicsCommandList::SetViewport(const Viewport& viewport)
{
    POMDOG_ASSERT(nativeCommandList);
    POMDOG_ASSERT(viewport.Width > 0);
    POMDOG_ASSERT(viewport.Height > 0);

    nativeCommandList->SetViewport(viewport);
}

void GraphicsCommandList::SetScissorRectangle(const Rectangle& rectangle)
{
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->SetScissorRectangle(rectangle);
}

void GraphicsCommandList::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
{
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->SetPrimitiveTopology(primitiveTopology);
}

void GraphicsCommandList::SetBlendFactor(const Color& blendFactor)
{
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->SetBlendFactor(blendFactor);
}

void GraphicsCommandList::SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
    POMDOG_ASSERT(vertexBuffer);
    POMDOG_ASSERT(nativeCommandList);

    nativeCommandList->SetVertexBuffers({
        VertexBufferBinding{vertexBuffer, 0}});
}

void GraphicsCommandList::SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, std::size_t offset)
{
    POMDOG_ASSERT(vertexBuffer);
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->SetVertexBuffers({
        VertexBufferBinding{vertexBuffer, offset}});
}

void GraphicsCommandList::SetVertexBuffers(const std::vector<VertexBufferBinding>& vertexBuffers)
{
    POMDOG_ASSERT(!vertexBuffers.empty());
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->SetVertexBuffers(vertexBuffers);
}

void GraphicsCommandList::SetVertexBuffers(std::vector<VertexBufferBinding> && vertexBuffers)
{
    POMDOG_ASSERT(!vertexBuffers.empty());
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->SetVertexBuffers(std::move(vertexBuffers));
}

void GraphicsCommandList::SetPipelineState(const std::shared_ptr<PipelineState>& pipelineState)
{
    POMDOG_ASSERT(pipelineState);
    POMDOG_ASSERT(nativeCommandList);

    using Detail::NativePipelineState;
    std::shared_ptr<NativePipelineState> nativePipelineState(
        pipelineState, pipelineState->GetNativePipelineState());

    nativeCommandList->SetPipelineState(nativePipelineState);
}

void GraphicsCommandList::SetConstantBuffer(int index, const std::shared_ptr<ConstantBuffer>& constantBuffer)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(constantBuffer);
    POMDOG_ASSERT(nativeCommandList);

    std::shared_ptr<Detail::NativeBuffer> nativeConstantBuffer(
        constantBuffer, constantBuffer->GetNativeConstantBuffer());

    nativeCommandList->SetConstantBuffer(index, nativeConstantBuffer);
}

void GraphicsCommandList::SetTexture(int index)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->SetTexture(index);
}

void GraphicsCommandList::SetTexture(int index, const std::shared_ptr<Texture2D>& texture)
{
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->SetTexture(index, texture);
}

void GraphicsCommandList::SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture)
{
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->SetTexture(index, texture);
}

void GraphicsCommandList::SetSamplerState(int index, const std::shared_ptr<SamplerState>& samplerState)
{
    POMDOG_ASSERT(samplerState);
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(samplerState->GetNativeSamplerState());
    POMDOG_ASSERT(nativeCommandList);

    std::shared_ptr<Detail::NativeSamplerState> nativeSamplerState(
        samplerState, samplerState->GetNativeSamplerState());

    nativeCommandList->SetSampler(index, std::move(nativeSamplerState));
}

void GraphicsCommandList::SetRenderTarget()
{
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->SetRenderTarget();
}

void GraphicsCommandList::SetRenderTarget(const std::shared_ptr<RenderTarget2D>& renderTarget)
{
    POMDOG_ASSERT(renderTarget);
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->SetRenderTargets({renderTarget});
}

void GraphicsCommandList::SetRenderTargets(const std::vector<std::shared_ptr<RenderTarget2D>>& renderTargets)
{
    POMDOG_ASSERT(!renderTargets.empty());
    POMDOG_ASSERT(nativeCommandList);

    if (renderTargets.empty()) {
        nativeCommandList->SetRenderTarget();
        return;
    }

    nativeCommandList->SetRenderTargets(renderTargets);
}

void GraphicsCommandList::SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets)
{
    POMDOG_ASSERT(!renderTargets.empty());
    POMDOG_ASSERT(nativeCommandList);

    if (renderTargets.empty()) {
        nativeCommandList->SetRenderTarget();
        return;
    }

    nativeCommandList->SetRenderTargets(std::move(renderTargets));
}

Detail::NativeGraphicsCommandList* GraphicsCommandList::GetNativeGraphicsCommandList()
{
    POMDOG_ASSERT(nativeCommandList);
    return nativeCommandList.get();
}

} // namespace Pomdog
