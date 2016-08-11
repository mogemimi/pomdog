// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "GraphicsContext.hpp"
#include "../RenderSystem/GraphicsCapabilities.hpp"
#include "../RenderSystem/NativeGraphicsContext.hpp"
#include "Pomdog/Graphics/ClearOptions.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
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
            POMDOG_ASSERT(renderTarget != texture);
        }
    }
}
#endif

ClearOptions ToClearOptions(DepthFormat depthFormat) noexcept
{
    switch (depthFormat) {
    case DepthFormat::Depth24Stencil8:
    case DepthFormat::Depth32_Float_Stencil8_Uint:
        return ClearOptions::DepthBuffer | ClearOptions::Stencil;
    case DepthFormat::Depth32:
    case DepthFormat::Depth16:
        return ClearOptions::DepthBuffer;
    default:
        break;
    }
    return ClearOptions::RenderTarget;
}

} // unnamed namespace

GraphicsContext::GraphicsContext(
    std::unique_ptr<Detail::NativeGraphicsContext> && nativeContextIn,
    const PresentationParameters& presentationParameters)
    : nativeContext(std::move(nativeContextIn))
    , depthStencilFormat(presentationParameters.DepthStencilFormat)
{
    POMDOG_ASSERT(nativeContext);
    auto graphicsCapbilities = nativeContext->GetCapabilities();

    POMDOG_ASSERT(graphicsCapbilities.SamplerSlotCount > 0);
    textures.clear();
    textures.resize(graphicsCapbilities.SamplerSlotCount);

    Viewport viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = presentationParameters.BackBufferWidth;
    viewport.Height = presentationParameters.BackBufferHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    nativeContext->SetViewport(viewport);
    nativeContext->SetScissorRectangle(viewport.GetBounds());
    nativeContext->SetBlendFactor(Color::White);
}

GraphicsContext::~GraphicsContext()
{
    textures.clear();
    renderTargets.clear();

    nativeContext.reset();
}

void GraphicsContext::Clear(const Color& color)
{
    ClearOptions options = ClearOptions::RenderTarget;

    if (!renderTargets.empty()) {
        auto & renderTarget = renderTargets.front();
        options |= ToClearOptions(renderTarget->GetDepthStencilFormat());
    }
    else {
        options |= ToClearOptions(depthStencilFormat);
    }

    POMDOG_ASSERT(nativeContext);
    nativeContext->Clear(options, color, 1.0f, 0);
}

void GraphicsContext::Clear(ClearOptions options, const Color& color, float depth, std::uint8_t stencil)
{
    POMDOG_ASSERT(nativeContext);
    nativeContext->Clear(options, color, depth, stencil);
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

void GraphicsContext::SetViewport(const Pomdog::Viewport& viewport)
{
    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(viewport.Width > 0);
    POMDOG_ASSERT(viewport.Height > 0);
    nativeContext->SetViewport(viewport);
}

void GraphicsContext::SetScissorRectangle(const Pomdog::Rectangle& rectangle)
{
    POMDOG_ASSERT(nativeContext);
    nativeContext->SetScissorRectangle(rectangle);
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

void GraphicsContext::SetRenderTarget()
{
    POMDOG_ASSERT(nativeContext);
    nativeContext->SetRenderTarget();
    renderTargets.clear();
}

void GraphicsContext::SetRenderTargets(const std::vector<std::shared_ptr<RenderTarget2D>>& renderTargetsIn)
{
    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(!renderTargetsIn.empty());
    renderTargets = renderTargetsIn;
    nativeContext->SetRenderTargets(renderTargets);
}

void GraphicsContext::SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargetsIn)
{
    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(!renderTargetsIn.empty());
    renderTargets = std::move(renderTargetsIn);
    nativeContext->SetRenderTargets(renderTargets);
}

Detail::NativeGraphicsContext* GraphicsContext::GetNativeGraphicsContext()
{
    POMDOG_ASSERT(nativeContext);
    return nativeContext.get();
}

} // namespace Detail
} // namespace Pomdog
