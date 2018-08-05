// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "../RenderSystem/NativeGraphicsCommandList.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/RenderPass.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#include "Pomdog/Graphics/VertexBufferBinding.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Rectangle.hpp"
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
{
}

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

void GraphicsCommandList::Draw(
    std::size_t vertexCount,
    std::size_t startVertexLocation)
{
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->Draw(vertexCount, startVertexLocation);
}

void GraphicsCommandList::DrawIndexed(
    const std::shared_ptr<IndexBuffer>& indexBuffer,
    std::size_t indexCount,
    std::size_t startIndexLocation)
{
    POMDOG_ASSERT(indexBuffer);
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->SetIndexBuffer(indexBuffer);
    nativeCommandList->DrawIndexed(indexCount, startIndexLocation);
}

void GraphicsCommandList::DrawInstanced(
    std::size_t vertexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startVertexLocation,
    std::size_t startInstanceLocation)
{
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->DrawInstanced(
        vertexCountPerInstance,
        instanceCount,
        startVertexLocation,
        startInstanceLocation);
}

void GraphicsCommandList::DrawIndexedInstanced(
    const std::shared_ptr<IndexBuffer>& indexBuffer,
    std::size_t indexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startIndexLocation,
    std::size_t startInstanceLocation)
{
    POMDOG_ASSERT(indexBuffer);
    POMDOG_ASSERT(nativeCommandList);
    nativeCommandList->SetIndexBuffer(indexBuffer);
    nativeCommandList->DrawIndexedInstanced(
        indexCountPerInstance,
        instanceCount,
        startIndexLocation,
        startInstanceLocation);
}

void GraphicsCommandList::SetRenderPass(RenderPass && renderPass)
{
    POMDOG_ASSERT(nativeCommandList);

#if defined(DEBUG) && !defined(NDEBUG)
    if (renderPass.Viewport) {
        const auto& viewport = *renderPass.Viewport;
        POMDOG_ASSERT(viewport.Width > 0);
        POMDOG_ASSERT(viewport.Height > 0);

        // NOTE: The MinDepth and MaxDepth must be between 0.0 and 1.0, respectively.
        // Please see the following article(s):
        // - https://developer.apple.com/documentation/metal/mtlrendercommandencoder/1515527-setviewport
        // - https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDepthRange.xhtml
        // - https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkViewport.html
        // - https://msdn.microsoft.com/en-us/library/windows/desktop/ff476260(v=vs.85).aspx
        POMDOG_ASSERT((0.0f <= viewport.MinDepth) && (viewport.MinDepth <= 1.0f));
        POMDOG_ASSERT((0.0f <= viewport.MaxDepth) && (viewport.MaxDepth <= 1.0f));
    }
    if (renderPass.ScissorRect) {
        const auto& scissorRect = *renderPass.ScissorRect;
        POMDOG_ASSERT(scissorRect.Width > 0);
        POMDOG_ASSERT(scissorRect.Height > 0);
    }
#endif

    nativeCommandList->SetRenderPass(std::move(renderPass));
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

Detail::NativeGraphicsCommandList* GraphicsCommandList::GetNativeGraphicsCommandList()
{
    POMDOG_ASSERT(nativeCommandList);
    return nativeCommandList.get();
}

} // namespace Pomdog
