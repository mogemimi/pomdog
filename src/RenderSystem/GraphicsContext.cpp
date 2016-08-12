// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "GraphicsContext.hpp"
#include "../RenderSystem/GraphicsCapabilities.hpp"
#include "../RenderSystem/NativeGraphicsContext.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/RenderPass.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
namespace {

#if defined(DEBUG) && !defined(NDEBUG)
void CheckUnbindingRenderTargetsError(
    const std::vector<std::shared_ptr<RenderTarget2D>>& renderTargets,
    const std::vector<std::shared_ptr<Texture>>& textures)
{
    for (auto & renderTarget: renderTargets) {
        for (auto & texture: textures) {
            POMDOG_ASSERT((renderTarget == nullptr) || (renderTarget != texture));
        }
    }
}
#endif

} // unnamed namespace

GraphicsContext::GraphicsContext(
    std::unique_ptr<Detail::NativeGraphicsContext> && nativeContextIn)
    : nativeContext(std::move(nativeContextIn))
{
    POMDOG_ASSERT(nativeContext);
    auto graphicsCapbilities = nativeContext->GetCapabilities();

    POMDOG_ASSERT(graphicsCapbilities.SamplerSlotCount > 0);
    textures.clear();
    textures.resize(graphicsCapbilities.SamplerSlotCount);

    nativeContext->SetBlendFactor(Color::White);
}

GraphicsContext::~GraphicsContext()
{
    textures.clear();
#if defined(DEBUG) && !defined(NDEBUG)
    renderTargets.clear();
#endif

    nativeContext.reset();
}

void GraphicsContext::Present()
{
    POMDOG_ASSERT(nativeContext);
    nativeContext->Present();
}

void GraphicsContext::Draw(std::size_t vertexCount)
{
#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(renderTargets, textures);
#endif

    POMDOG_ASSERT(nativeContext);
    nativeContext->Draw(vertexCount);
}

void GraphicsContext::DrawIndexed(std::size_t indexCount)
{
#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(renderTargets, textures);
#endif

    POMDOG_ASSERT(nativeContext);
    nativeContext->DrawIndexed(indexCount);
}

void GraphicsContext::DrawInstanced(
    std::size_t vertexCount, std::size_t instanceCount)
{
#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(renderTargets, textures);
#endif

    POMDOG_ASSERT(nativeContext);
    nativeContext->DrawInstanced(vertexCount, instanceCount);
}

void GraphicsContext::DrawIndexedInstanced(
    std::size_t indexCount, std::size_t instanceCount)
{
#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(renderTargets, textures);
#endif

    POMDOG_ASSERT(nativeContext);
    nativeContext->DrawIndexedInstanced(indexCount, instanceCount);
}

void GraphicsContext::SetRenderPass(const RenderPass& renderPass)
{
    POMDOG_ASSERT(nativeContext);

#if defined(DEBUG) && !defined(NDEBUG)
    renderTargets.clear();
    for (auto& renderTarget : renderPass.RenderTargets) {
        renderTargets.push_back(std::get<0>(renderTarget));
    }
#endif
    nativeContext->SetRenderPass(renderPass);
}

void GraphicsContext::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
{
    POMDOG_ASSERT(nativeContext);
    nativeContext->SetPrimitiveTopology(primitiveTopology);
}

void GraphicsContext::SetBlendFactor(const Color& blendFactor)
{
    POMDOG_ASSERT(nativeContext);
    nativeContext->SetBlendFactor(blendFactor);
}

void GraphicsContext::SetVertexBuffers(const std::vector<VertexBufferBinding>& vertexBuffers)
{
    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(!vertexBuffers.empty());
    nativeContext->SetVertexBuffers(vertexBuffers);
}

void GraphicsContext::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(indexBuffer);

    nativeContext->SetIndexBuffer(indexBuffer);
}

void GraphicsContext::SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState)
{
    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(pipelineState);

    nativeContext->SetPipelineState(pipelineState);
}

void GraphicsContext::SetConstantBuffer(int index, const std::shared_ptr<NativeBuffer>& constantBuffer)
{
    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(constantBuffer);

    nativeContext->SetConstantBuffer(index, constantBuffer);
}

void GraphicsContext::SetSampler(int index, const std::shared_ptr<NativeSamplerState>& sampler)
{
    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(textures.size()));
    POMDOG_ASSERT(sampler);

    if (index < static_cast<int>(textures.size())) {
        nativeContext->SetSampler(index, sampler.get());
    }
}

void GraphicsContext::SetTexture(int index)
{
    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(!textures.empty());
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(textures.size()));

    if (index < static_cast<int>(textures.size())) {
        nativeContext->SetTexture(index);
        textures[index].reset();
    }
}

void GraphicsContext::SetTexture(int index, const std::shared_ptr<Texture2D>& textureIn)
{
    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(textureIn);
    POMDOG_ASSERT(!textures.empty());
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(textures.size()));

    if (index < static_cast<int>(textures.size())) {
        textures[index] = textureIn;
        nativeContext->SetTexture(index, *textureIn);
    }
}

void GraphicsContext::SetTexture(int index, const std::shared_ptr<RenderTarget2D>& textureIn)
{
    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(textureIn);
    POMDOG_ASSERT(!textures.empty());
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(textures.size()));

    if (index < static_cast<int>(textures.size())) {
        textures[index] = textureIn;
        nativeContext->SetTexture(index, *textureIn);
    }
}

Detail::NativeGraphicsContext* GraphicsContext::GetNativeGraphicsContext()
{
    POMDOG_ASSERT(nativeContext);
    return nativeContext.get();
}

} // namespace Detail
} // namespace Pomdog
