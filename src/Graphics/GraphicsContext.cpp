// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/GraphicsContext.hpp"
#include "../RenderSystem/GraphicsCapabilities.hpp"
#include "../RenderSystem/NativeGraphicsContext.hpp"
#include "../RenderSystem/NativeBlendState.hpp"
#include "../RenderSystem/NativeDepthStencilState.hpp"
#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/BlendState.hpp"
#include "Pomdog/Graphics/ConstantBufferBinding.hpp"
#include "Pomdog/Graphics/DepthStencilState.hpp"
#include "Pomdog/Graphics/EffectPass.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Graphics/VertexBufferBinding.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <utility>

namespace Pomdog {
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

}// unnamed namespace
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

    Impl(std::unique_ptr<Detail::RenderSystem::NativeGraphicsContext> nativeContext,
        PresentationParameters const& presentationParameters);

    void BuildResources(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    ///@copydoc GraphicsContext
    void SetViewport(Pomdog::Viewport const& viewport);

    ///@copydoc GraphicsContext
    void SetBlendState(std::shared_ptr<BlendState> const& blendState);

    ///@copydoc GraphicsContext
    void SetDepthStencilState(std::shared_ptr<DepthStencilState> const& depthStencilState);

    ///@copydoc GraphicsContext
    void SetSamplerState(int samplerSlot, std::shared_ptr<SamplerState> const& samplerState);

    ///@copydoc GraphicsContext
    void SetTexture(int textureSlot);

    ///@copydoc GraphicsContext
    void SetTexture(int textureSlot, std::shared_ptr<Texture2D> const& texture);

    ///@copydoc GraphicsContext
    void SetTexture(int textureSlot, std::shared_ptr<RenderTarget2D> const& texture);

    ///@copydoc GraphicsContext
    void SetRenderTarget();

    ///@copydoc GraphicsContext
    void SetRenderTarget(std::shared_ptr<RenderTarget2D> const& renderTarget);

    ///@copydoc GraphicsContext
    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets);

    ///@copydoc GraphicsContext
    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets);

    ///@copydoc GraphicsContext
    void SetEffectPass(std::shared_ptr<EffectPass> const& effectPass);

    ///@copydoc GraphicsContext
    void SetConstantBuffers(std::shared_ptr<ConstantBufferBinding> const& constantBuffers);

public:
    Pomdog::Viewport viewport;
    std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers;
    std::vector<std::shared_ptr<SamplerState>> samplerStates;
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<std::shared_ptr<RenderTarget2D>> renderTargets;
    std::shared_ptr<BlendState> blendState;
    std::shared_ptr<DepthStencilState> depthStencilState;
    std::shared_ptr<EffectPass> effectPass;
    std::shared_ptr<ConstantBufferBinding> constantBuffers;

    std::unique_ptr<Detail::RenderSystem::NativeGraphicsContext> nativeContext;
};
//-----------------------------------------------------------------------
GraphicsContext::Impl::Impl(std::unique_ptr<Detail::RenderSystem::NativeGraphicsContext> nativeGraphicsContext,
    PresentationParameters const& presentationParameters)
    : nativeContext(std::move(nativeGraphicsContext))
{
    POMDOG_ASSERT(nativeContext);
    auto graphicsCapbilities = nativeContext->GetCapabilities();

    POMDOG_ASSERT(graphicsCapbilities.SamplerSlotCount > 0);
    samplerStates.clear();
    samplerStates.resize(graphicsCapbilities.SamplerSlotCount);
    textures.clear();
    textures.resize(graphicsCapbilities.SamplerSlotCount);

    viewport.Bounds.X = 0;
    viewport.Bounds.Y = 0;
    viewport.Width(presentationParameters.BackBufferWidth);
    viewport.Height(presentationParameters.BackBufferHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    SetViewport(viewport);
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::BuildResources(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(graphicsDevice);

    blendState = BlendState::CreateOpaque(graphicsDevice);
    depthStencilState = DepthStencilState::CreateReadWriteDepth(graphicsDevice);

    POMDOG_ASSERT(!samplerStates.empty());
    for (std::size_t index = 0; index < samplerStates.size(); ++index) {
        samplerStates[index] = SamplerState::CreateLinearClamp(graphicsDevice);
    }

    if (blendState) {
        SetBlendState(blendState);
    }
    if (depthStencilState) {
        SetDepthStencilState(depthStencilState);
    }
    for (std::uint32_t index = 0; index < samplerStates.size(); ++index) {
        if (samplerStates[index]) {
            SetSamplerState(index, samplerStates[index]);
        }
    }
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::SetViewport(Pomdog::Viewport const& newViewport)
{
    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(newViewport.Width() > 0);
    POMDOG_ASSERT(newViewport.Height() > 0);
    POMDOG_ASSERT(nativeContext);

    this->viewport = newViewport;
    nativeContext->SetViewport(this->viewport);
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::SetBlendState(std::shared_ptr<BlendState> const& blendStateIn)
{
    POMDOG_ASSERT(blendStateIn);
    blendState = blendStateIn;

    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(blendState->NativeBlendState());
    blendState->NativeBlendState()->Apply(*nativeContext);
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::SetDepthStencilState(std::shared_ptr<DepthStencilState> const& depthStencilStateIn)
{
    POMDOG_ASSERT(depthStencilStateIn);
    depthStencilState = depthStencilStateIn;

    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(depthStencilState->NativeDepthStencilState());
    depthStencilState->NativeDepthStencilState()->Apply(*nativeContext);
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::SetSamplerState(int samplerSlot, std::shared_ptr<SamplerState> const& samplerStateIn)
{
    POMDOG_ASSERT(samplerStateIn);
    POMDOG_ASSERT(!samplerStates.empty());

    POMDOG_ASSERT(samplerSlot >= 0);
    POMDOG_ASSERT(samplerSlot < static_cast<int>(samplerStates.size()));

    if (samplerSlot < static_cast<int>(samplerStates.size()))
    {
        samplerStates[samplerSlot] = samplerStateIn;

        POMDOG_ASSERT(nativeContext);
        POMDOG_ASSERT(samplerStateIn->NativeSamplerState());
        samplerStateIn->NativeSamplerState()->Apply(*nativeContext, samplerSlot);
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
void GraphicsContext::Impl::SetRenderTarget(std::shared_ptr<RenderTarget2D> const& renderTargetIn)
{
    POMDOG_ASSERT(renderTargetIn);
    POMDOG_ASSERT(nativeContext);
    renderTargets.clear();
    renderTargets.push_back(renderTargetIn);
    nativeContext->SetRenderTargets(renderTargets);
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
void GraphicsContext::Impl::SetEffectPass(std::shared_ptr<EffectPass> const& effectPassIn)
{
    POMDOG_ASSERT(effectPassIn);
    POMDOG_ASSERT(nativeContext);
    effectPass = effectPassIn;

    using Detail::RenderSystem::NativeEffectPass;
    std::shared_ptr<NativeEffectPass> nativeEffectPass(effectPass, effectPass->NativeEffectPass());
    nativeContext->SetEffectPass(nativeEffectPass);
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::SetConstantBuffers(std::shared_ptr<ConstantBufferBinding> const& constantBuffersIn)
{
    POMDOG_ASSERT(constantBuffersIn);
    POMDOG_ASSERT(nativeContext);
    constantBuffers = constantBuffersIn;

    using Detail::RenderSystem::NativeConstantLayout;
    std::shared_ptr<NativeConstantLayout> nativeConstantLayout(constantBuffers, constantBuffers->NativeConstantLayout());
    nativeContext->SetConstantBuffers(nativeConstantLayout);
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - GraphicsContext class
#endif
//-----------------------------------------------------------------------
GraphicsContext::GraphicsContext(
    std::unique_ptr<Detail::RenderSystem::NativeGraphicsContext> nativeContext,
    PresentationParameters const& presentationParameters,
    std::shared_ptr<GraphicsDevice> const& graphicsDevice)
    : impl(std::make_unique<Impl>(std::move(nativeContext), presentationParameters))
{
    POMDOG_ASSERT(graphicsDevice);
    impl->BuildResources(graphicsDevice);
}
//-----------------------------------------------------------------------
GraphicsContext::~GraphicsContext() = default;
//-----------------------------------------------------------------------
void GraphicsContext::Clear(Color const& color)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    impl->nativeContext->Clear(color);
}
//-----------------------------------------------------------------------
void GraphicsContext::Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil)
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
void GraphicsContext::Draw(PrimitiveTopology primitiveTopology, std::size_t vertexCount)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(impl->renderTargets, impl->textures);
#endif

    impl->nativeContext->Draw(primitiveTopology, vertexCount);
}
//-----------------------------------------------------------------------
void GraphicsContext::DrawIndexed(PrimitiveTopology primitiveTopology,
    std::shared_ptr<IndexBuffer> const& indexBuffer, std::size_t indexCount)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(impl->renderTargets, impl->textures);
#endif

    impl->nativeContext->DrawIndexed(primitiveTopology, indexBuffer, indexCount);
}
//-----------------------------------------------------------------------
void GraphicsContext::DrawInstanced(PrimitiveTopology primitiveTopology,
    std::size_t vertexCount, std::size_t instanceCount)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(impl->renderTargets, impl->textures);
#endif

    impl->nativeContext->DrawInstanced(primitiveTopology, vertexCount, instanceCount);
}
//-----------------------------------------------------------------------
void GraphicsContext::DrawIndexedInstanced(PrimitiveTopology primitiveTopology,
    std::shared_ptr<IndexBuffer> const& indexBuffer, std::size_t indexCount, std::size_t instanceCount)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(impl->renderTargets, impl->textures);
#endif

    impl->nativeContext->DrawIndexedInstanced(primitiveTopology, indexBuffer, indexCount, instanceCount);
}
//-----------------------------------------------------------------------
Viewport const& GraphicsContext::Viewport() const
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    return impl->viewport;
}
//-----------------------------------------------------------------------
void GraphicsContext::Viewport(Pomdog::Viewport const& viewport)
{
    POMDOG_ASSERT(impl);
    impl->SetViewport(viewport);
}
//-----------------------------------------------------------------------
Rectangle GraphicsContext::ScissorRectangle() const
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    return impl->nativeContext->GetScissorRectangle();
}
//-----------------------------------------------------------------------
void GraphicsContext::ScissorRectangle(Pomdog::Rectangle const& rectangle)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    impl->nativeContext->SetScissorRectangle(rectangle);
}
//-----------------------------------------------------------------------
std::shared_ptr<BlendState> GraphicsContext::GetBlendState() const
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->blendState);
    return impl->blendState;
}
//-----------------------------------------------------------------------
void GraphicsContext::SetBlendState(std::shared_ptr<BlendState> const& blendState)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(blendState);
    impl->SetBlendState(blendState);
}
//-----------------------------------------------------------------------
std::shared_ptr<DepthStencilState> GraphicsContext::GetDepthStencilState() const
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->depthStencilState);
    return impl->depthStencilState;
}
//-----------------------------------------------------------------------
void GraphicsContext::SetDepthStencilState(std::shared_ptr<DepthStencilState> const& depthStencilState)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(depthStencilState);
    impl->SetDepthStencilState(depthStencilState);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetSamplerState(int index, std::shared_ptr<SamplerState> const& samplerState)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(samplerState);
    impl->SetSamplerState(index, samplerState);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetVertexBuffer(std::shared_ptr<VertexBuffer> const& vertexBuffer)
{
    POMDOG_ASSERT(vertexBuffer);
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    impl->vertexBuffers.clear();
    impl->vertexBuffers.push_back(vertexBuffer);

    impl->nativeContext->SetVertexBuffers(impl->vertexBuffers);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers)
{
    POMDOG_ASSERT(!vertexBuffers.empty());
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    impl->vertexBuffers = vertexBuffers;
    impl->nativeContext->SetVertexBuffers(impl->vertexBuffers);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> && vertexBuffers)
{
    POMDOG_ASSERT(!vertexBuffers.empty());
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    impl->vertexBuffers = std::move(vertexBuffers);
    impl->nativeContext->SetVertexBuffers(impl->vertexBuffers);
}
//-----------------------------------------------------------------------
std::shared_ptr<Texture> GraphicsContext::GetTexture(int index) const
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    POMDOG_ASSERT(!impl->textures.empty());
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(impl->textures.size()));

    return impl->textures[index];
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
std::vector<std::shared_ptr<RenderTarget2D>> GraphicsContext::GetRenderTargets() const
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    return impl->renderTargets;
}
//-----------------------------------------------------------------------
void GraphicsContext::SetRenderTarget()
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    impl->SetRenderTarget();
}
//-----------------------------------------------------------------------
void GraphicsContext::SetRenderTarget(std::shared_ptr<RenderTarget2D> const& renderTarget)
{
    POMDOG_ASSERT(renderTarget);
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    impl->SetRenderTarget(renderTarget);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets)
{
    POMDOG_ASSERT(!renderTargets.empty());
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    if (renderTargets.empty())
    {
        impl->SetRenderTarget();
        return;
    }

    impl->SetRenderTargets(renderTargets);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets)
{
    POMDOG_ASSERT(!renderTargets.empty());
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

    if (renderTargets.empty())
    {
        impl->SetRenderTarget();
        return;
    }

    impl->SetRenderTargets(std::move(renderTargets));
}
//-----------------------------------------------------------------------
void GraphicsContext::SetEffectPass(std::shared_ptr<EffectPass> const& effectPass)
{
    POMDOG_ASSERT(effectPass);
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    impl->SetEffectPass(effectPass);
}
//-----------------------------------------------------------------------
void GraphicsContext::SetConstantBuffers(std::shared_ptr<ConstantBufferBinding> const& constantBuffers)
{
    POMDOG_ASSERT(constantBuffers);
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    impl->SetConstantBuffers(constantBuffers);
}
//-----------------------------------------------------------------------
Detail::RenderSystem::NativeGraphicsContext* GraphicsContext::NativeGraphicsContext()
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    return impl->nativeContext.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
