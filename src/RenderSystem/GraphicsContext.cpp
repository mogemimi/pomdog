// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "GraphicsContext.hpp"
#include "../RenderSystem/GraphicsCapabilities.hpp"
#include "../RenderSystem/NativeGraphicsContext.hpp"
#include "../RenderSystem/NativeSamplerState.hpp"
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
static void CheckUnbindingRenderTargetsError(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets,
    std::vector<std::shared_ptr<Texture>> const& textures)
{
    for (auto & renderTarget: renderTargets) {
        for (auto & texture: textures) {
            POMDOG_ASSERT(renderTarget != texture);
        }
    }
}
#endif
//-----------------------------------------------------------------------
static ClearOptions ToClearOptions(DepthFormat depthFormat) noexcept
{
    switch (depthFormat) {
    case DepthFormat::Depth24Stencil8:
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
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - GraphicsContext::Impl class
#endif
//-----------------------------------------------------------------------
class GraphicsContext::Impl final {
public:
    Impl() = delete;
    Impl(Impl const&) = delete;
    Impl(Impl &&) = default;

    Impl(std::unique_ptr<Detail::NativeGraphicsContext> nativeContext,
        PresentationParameters const& presentationParameters);

    ~Impl();

    void SetSampler(int samplerSlot, std::shared_ptr<Detail::NativeSamplerState> const& sampler);

    void SetTexture(int textureSlot);

    void SetTexture(int textureSlot, std::shared_ptr<Texture2D> const& texture);

    void SetTexture(int textureSlot, std::shared_ptr<RenderTarget2D> const& texture);

    void SetRenderTarget();

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets);

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets);

public:
    std::shared_ptr<Detail::NativePipelineState> pipelineState;
    std::shared_ptr<Detail::NativeConstantLayout> constantBuffers;
    std::vector<std::shared_ptr<Detail::NativeSamplerState>> samplerStates;
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<std::shared_ptr<RenderTarget2D>> renderTargets;
    std::unique_ptr<Detail::NativeGraphicsContext> nativeContext;
    DepthFormat depthStencilFormat;
};
//-----------------------------------------------------------------------
GraphicsContext::Impl::Impl(std::unique_ptr<Detail::NativeGraphicsContext> nativeGraphicsContext,
    PresentationParameters const& presentationParameters)
    : nativeContext(std::move(nativeGraphicsContext))
    , depthStencilFormat(presentationParameters.DepthStencilFormat)
{
    POMDOG_ASSERT(nativeContext);
    auto graphicsCapbilities = nativeContext->GetCapabilities();

    POMDOG_ASSERT(graphicsCapbilities.SamplerSlotCount > 0);
    samplerStates.clear();
    samplerStates.resize(graphicsCapbilities.SamplerSlotCount);
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
//-----------------------------------------------------------------------
GraphicsContext::Impl::~Impl()
{
    constantBuffers.reset();
    pipelineState.reset();
    samplerStates.clear();
    textures.clear();
    renderTargets.clear();

    nativeContext.reset();
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::SetSampler(int samplerSlot, std::shared_ptr<Detail::NativeSamplerState> const& samplerIn)
{
    POMDOG_ASSERT(samplerIn);
    POMDOG_ASSERT(!samplerStates.empty());

    POMDOG_ASSERT(samplerSlot >= 0);
    POMDOG_ASSERT(samplerSlot < static_cast<int>(samplerStates.size()));

    if (samplerSlot < static_cast<int>(samplerStates.size()))
    {
        samplerStates[samplerSlot] = samplerIn;

        POMDOG_ASSERT(nativeContext);
        nativeContext->SetSampler(samplerSlot, samplerIn.get());
    }
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::SetTexture(int textureSlot)
{
    POMDOG_ASSERT(!textures.empty());
    POMDOG_ASSERT(textureSlot >= 0);
    POMDOG_ASSERT(textureSlot < static_cast<int>(textures.size()));
    POMDOG_ASSERT(nativeContext);

    if (textureSlot < static_cast<int>(textures.size()))
    {
        nativeContext->SetTexture(textureSlot);
        textures[textureSlot].reset();
    }
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::SetTexture(int textureSlot, std::shared_ptr<Texture2D> const& textureIn)
{
    POMDOG_ASSERT(textureIn);
    POMDOG_ASSERT(!textures.empty());
    POMDOG_ASSERT(textureSlot >= 0);
    POMDOG_ASSERT(textureSlot < static_cast<int>(textures.size()));
    POMDOG_ASSERT(nativeContext);

    if (textureSlot < static_cast<int>(textures.size()))
    {
        textures[textureSlot] = textureIn;
        nativeContext->SetTexture(textureSlot, *textureIn);
    }
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::SetTexture(int textureSlot, std::shared_ptr<RenderTarget2D> const& textureIn)
{
    POMDOG_ASSERT(textureIn);
    POMDOG_ASSERT(!textures.empty());
    POMDOG_ASSERT(textureSlot >= 0);
    POMDOG_ASSERT(textureSlot < static_cast<int>(textures.size()));

    if (textureSlot < static_cast<int>(textures.size()))
    {
        textures[textureSlot] = textureIn;
        nativeContext->SetTexture(textureSlot, *textureIn);
    }
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::SetRenderTarget()
{
    POMDOG_ASSERT(nativeContext);
    nativeContext->SetRenderTarget();
    renderTargets.clear();
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargetsIn)
{
    POMDOG_ASSERT(!renderTargetsIn.empty());
    POMDOG_ASSERT(nativeContext);
    renderTargets = renderTargetsIn;
    nativeContext->SetRenderTargets(renderTargets);
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargetsIn)
{
    POMDOG_ASSERT(!renderTargetsIn.empty());
    POMDOG_ASSERT(nativeContext);
    renderTargets = std::move(renderTargetsIn);
    nativeContext->SetRenderTargets(renderTargets);
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - GraphicsContext class
#endif
//-----------------------------------------------------------------------
GraphicsContext::GraphicsContext(
    std::unique_ptr<Detail::NativeGraphicsContext> && nativeContext,
    PresentationParameters const& presentationParameters)
    : impl(std::make_unique<Impl>(std::move(nativeContext), presentationParameters))
{
}
//-----------------------------------------------------------------------
GraphicsContext::~GraphicsContext() = default;
//-----------------------------------------------------------------------
void GraphicsContext::Clear(Color const& color)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    ClearOptions options = ClearOptions::RenderTarget;

    if (!impl->renderTargets.empty()) {
        auto & renderTarget = impl->renderTargets.front();
        options |= ToClearOptions(renderTarget->DepthStencilFormat());
    }
    else {
        options |= ToClearOptions(impl->depthStencilFormat);
    }

    impl->nativeContext->Clear(options, color, 1.0f, 0);
}
//-----------------------------------------------------------------------
void GraphicsContext::Clear(ClearOptions options, Color const& color, float depth, std::uint8_t stencil)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    impl->nativeContext->Clear(options, color, depth, stencil);
}
//-----------------------------------------------------------------------
void GraphicsContext::Present()
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    impl->nativeContext->Present();
}
//-----------------------------------------------------------------------
void GraphicsContext::Draw(std::size_t vertexCount)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(impl->renderTargets, impl->textures);
#endif

    impl->nativeContext->Draw(vertexCount);
}
//-----------------------------------------------------------------------
void GraphicsContext::DrawIndexed(std::size_t indexCount)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(impl->renderTargets, impl->textures);
#endif

    impl->nativeContext->DrawIndexed(indexCount);
}
//-----------------------------------------------------------------------
void GraphicsContext::DrawInstanced(
    std::size_t vertexCount, std::size_t instanceCount)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(impl->renderTargets, impl->textures);
#endif

    impl->nativeContext->DrawInstanced(vertexCount, instanceCount);
}
//-----------------------------------------------------------------------
void GraphicsContext::DrawIndexedInstanced(
    std::size_t indexCount, std::size_t instanceCount)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(impl->renderTargets, impl->textures);
#endif

    impl->nativeContext->DrawIndexedInstanced(indexCount, instanceCount);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetViewport(Pomdog::Viewport const& viewport)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    POMDOG_ASSERT(viewport.Width > 0);
    POMDOG_ASSERT(viewport.Height > 0);
    impl->nativeContext->SetViewport(viewport);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetScissorRectangle(Pomdog::Rectangle const& rectangle)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    impl->nativeContext->SetScissorRectangle(rectangle);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    impl->nativeContext->SetPrimitiveTopology(primitiveTopology);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetBlendFactor(Color const& blendFactor)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    impl->nativeContext->SetBlendFactor(blendFactor);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    POMDOG_ASSERT(!vertexBuffers.empty());
    impl->nativeContext->SetVertexBuffers(vertexBuffers);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetIndexBuffer(std::shared_ptr<IndexBuffer> const& indexBuffer)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    POMDOG_ASSERT(indexBuffer);

    impl->nativeContext->SetIndexBuffer(indexBuffer);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetPipelineState(std::shared_ptr<Detail::NativePipelineState> const& pipelineState)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    POMDOG_ASSERT(pipelineState);

    impl->nativeContext->SetPipelineState(pipelineState);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetConstantBuffers(std::shared_ptr<Detail::NativeConstantLayout> const& constantBuffers)
{
    POMDOG_ASSERT(constantBuffers);
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    impl->nativeContext->SetConstantBuffers(constantBuffers);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetSampler(int index, std::shared_ptr<Detail::NativeSamplerState> const& sampler)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(sampler);

    impl->SetSampler(index, sampler);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetTexture(int index)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    impl->SetTexture(index);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetTexture(int index, std::shared_ptr<Texture2D> const& texture)
{
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    impl->SetTexture(index, texture);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetTexture(int index, std::shared_ptr<RenderTarget2D> const& texture)
{
    POMDOG_ASSERT(texture);
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    impl->SetTexture(index, texture);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetRenderTarget()
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    impl->SetRenderTarget();
}
//-----------------------------------------------------------------------
void GraphicsContext::SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets)
{
    POMDOG_ASSERT(!renderTargets.empty());
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    impl->SetRenderTargets(renderTargets);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets)
{
    POMDOG_ASSERT(!renderTargets.empty());
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    impl->SetRenderTargets(std::move(renderTargets));
}
//-----------------------------------------------------------------------
Detail::NativeGraphicsContext* GraphicsContext::GetNativeGraphicsContext()
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    return impl->nativeContext.get();
}
//-----------------------------------------------------------------------
} // namespace Detail
} // namespace Pomdog
