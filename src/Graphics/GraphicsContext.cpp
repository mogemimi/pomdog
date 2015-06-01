// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/GraphicsContext.hpp"
#include "../RenderSystem/GraphicsCapabilities.hpp"
#include "../RenderSystem/NativeGraphicsContext.hpp"
#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/ClearOptions.hpp"
#include "Pomdog/Graphics/ConstantBufferBinding.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Color.hpp"
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

    void BuildResources(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    void SetViewport(Viewport const& viewport);

    void SetSamplerState(int samplerSlot, std::shared_ptr<SamplerState> const& samplerState);

    void SetTexture(int textureSlot);

    void SetTexture(int textureSlot, std::shared_ptr<Texture2D> const& texture);

    void SetTexture(int textureSlot, std::shared_ptr<RenderTarget2D> const& texture);

    void SetRenderTarget();

    void SetRenderTarget(std::shared_ptr<RenderTarget2D> const& renderTarget);

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets);

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets);

    void SetPipelineState(std::shared_ptr<PipelineState> const& pipelineState);

    void SetConstantBuffers(std::shared_ptr<ConstantBufferBinding> const& constantBuffers);

public:
    Viewport viewport;
    Rectangle scissorRectangle;
    std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers;
    std::vector<std::shared_ptr<SamplerState>> samplerStates;
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<std::shared_ptr<RenderTarget2D>> renderTargets;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<ConstantBufferBinding> constantBuffers;

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

    viewport.Bounds.X = 0;
    viewport.Bounds.Y = 0;
    viewport.Bounds.Width = presentationParameters.BackBufferWidth;
    viewport.Bounds.Height = presentationParameters.BackBufferHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    SetViewport(viewport);

    nativeContext->SetScissorRectangle(viewport.Bounds);
    nativeContext->SetBlendFactor(Color::White);
}
//-----------------------------------------------------------------------
GraphicsContext::Impl::~Impl()
{
    constantBuffers.reset();
    pipelineState.reset();
    vertexBuffers.clear();
    samplerStates.clear();
    textures.clear();
    renderTargets.clear();

    nativeContext.reset();
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::BuildResources(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(graphicsDevice);

    POMDOG_ASSERT(!samplerStates.empty());

    auto samplerDesc = SamplerDescription::CreateDefault();
    for (std::size_t index = 0; index < samplerStates.size(); ++index) {
        samplerStates[index] = std::make_shared<SamplerState>(graphicsDevice, samplerDesc);
    }

    for (std::uint32_t index = 0; index < samplerStates.size(); ++index) {
        if (samplerStates[index]) {
            SetSamplerState(index, samplerStates[index]);
        }
    }
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::SetViewport(Pomdog::Viewport const& viewportIn)
{
    POMDOG_ASSERT(nativeContext);
    POMDOG_ASSERT(viewportIn.GetWidth() > 0);
    POMDOG_ASSERT(viewportIn.GetHeight() > 0);
    POMDOG_ASSERT(nativeContext);

    this->viewport = viewportIn;
    nativeContext->SetViewport(this->viewport);
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
        nativeContext->SetSampler(samplerSlot, samplerStateIn->NativeSamplerState());
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
void GraphicsContext::Impl::SetPipelineState(std::shared_ptr<PipelineState> const& pipelineStateIn)
{
    POMDOG_ASSERT(pipelineStateIn);
    POMDOG_ASSERT(nativeContext);
    pipelineState = pipelineStateIn;

    using Detail::NativePipelineState;
    std::shared_ptr<NativePipelineState> nativePipelineState(
        pipelineState, pipelineState->NativePipelineState());

    nativeContext->SetPipelineState(nativePipelineState);
}
//-----------------------------------------------------------------------
void GraphicsContext::Impl::SetConstantBuffers(std::shared_ptr<ConstantBufferBinding> const& constantBuffersIn)
{
    POMDOG_ASSERT(constantBuffersIn);
    POMDOG_ASSERT(nativeContext);
    constantBuffers = constantBuffersIn;

    using Detail::NativeConstantLayout;
    std::shared_ptr<NativeConstantLayout> nativeConstantLayout(constantBuffers, constantBuffers->NativeConstantLayout());
    nativeContext->SetConstantBuffers(nativeConstantLayout);
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - GraphicsContext class
#endif
//-----------------------------------------------------------------------
GraphicsContext::GraphicsContext(
    std::unique_ptr<Detail::NativeGraphicsContext> && nativeContext,
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
void GraphicsContext::DrawIndexed(
    std::shared_ptr<IndexBuffer> const& indexBuffer, std::size_t indexCount)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(impl->renderTargets, impl->textures);
#endif

    impl->nativeContext->SetIndexBuffer(indexBuffer);
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
    std::shared_ptr<IndexBuffer> const& indexBuffer,
    std::size_t indexCount, std::size_t instanceCount)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);

#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(impl->renderTargets, impl->textures);
#endif

    impl->nativeContext->SetIndexBuffer(indexBuffer);
    impl->nativeContext->DrawIndexedInstanced(indexCount, instanceCount);
}
//-----------------------------------------------------------------------
Viewport GraphicsContext::GetViewport() const
{
    POMDOG_ASSERT(impl);
    return impl->viewport;
}
//-----------------------------------------------------------------------
void GraphicsContext::SetViewport(Pomdog::Viewport const& viewport)
{
    POMDOG_ASSERT(impl);
    impl->SetViewport(viewport);
}
//-----------------------------------------------------------------------
Rectangle GraphicsContext::GetScissorRectangle() const
{
    POMDOG_ASSERT(impl);
    return impl->scissorRectangle;
}
//-----------------------------------------------------------------------
void GraphicsContext::SetScissorRectangle(Pomdog::Rectangle const& rectangle)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    impl->scissorRectangle = rectangle;
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
void GraphicsContext::SetPipelineState(std::shared_ptr<PipelineState> const& pipelineState)
{
    POMDOG_ASSERT(pipelineState);
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    impl->SetPipelineState(pipelineState);
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
Detail::NativeGraphicsContext* GraphicsContext::NativeGraphicsContext()
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeContext);
    return impl->nativeContext.get();
}
//-----------------------------------------------------------------------
} // namespace Pomdog
