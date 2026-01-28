// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d11/graphics_context_direct3d11.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/command_list_immediate.h"
#include "pomdog/gpu/backends/graphics_capabilities.h"
#include "pomdog/gpu/direct3d11/buffer_direct3d11.h"
#include "pomdog/gpu/direct3d11/depth_stencil_buffer_direct3d11.h"
#include "pomdog/gpu/direct3d11/pipeline_state_direct3d11.h"
#include "pomdog/gpu/direct3d11/render_target2d_direct3d11.h"
#include "pomdog/gpu/direct3d11/sampler_state_direct3d11.h"
#include "pomdog/gpu/direct3d11/texture2d_direct3d11.h"
#include "pomdog/gpu/dxgi/dxgi_format_helper.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/render_pass.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/logging/log.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/math/vector4.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {
namespace {

using Microsoft::WRL::ComPtr;

void chooseMultiSampleSetting(
    ID3D11Device* device,
    DXGI_FORMAT backBufferFormat,
    int preferredMultiSampleCount,
    DXGI_SAMPLE_DESC& sampleDesc)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(preferredMultiSampleCount >= 1);
    POMDOG_ASSERT(preferredMultiSampleCount <= 32);

    const auto maxSampleCount = std::min(
        preferredMultiSampleCount,
        D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT);

    for (int sampleCount = maxSampleCount; sampleCount >= 1; --sampleCount) {
        UINT qualityLevels = 0;

        auto hr = device->CheckMultisampleQualityLevels(
            backBufferFormat, sampleCount, &qualityLevels);

        if (SUCCEEDED(hr) && (0 < qualityLevels)) {
            sampleDesc.Count = sampleCount;
            sampleDesc.Quality = qualityLevels - 1;
            return;
        }
    }
}

void useBackBufferAsRenderTarget(
    Microsoft::WRL::ComPtr<ID3D11DeviceContext3>& deferredContext,
    std::vector<std::shared_ptr<RenderTarget2DDirect3D11>>& renderTargets,
    const std::shared_ptr<RenderTarget2DDirect3D11>& backBuffer,
    const std::shared_ptr<DepthStencilBufferDirect3D11>& depthStencilBuffer)
{
    POMDOG_ASSERT(deferredContext);
    POMDOG_ASSERT(backBuffer);

    renderTargets.clear();
    renderTargets.push_back(backBuffer);

    std::array<ID3D11RenderTargetView*, 1> renderTargetViews = {
        backBuffer->getRenderTargetView()};

    ID3D11DepthStencilView* depthStencilView = nullptr;
    if (depthStencilBuffer != nullptr) {
        depthStencilView = depthStencilBuffer->getDepthStencilView();
    }

    deferredContext->OMSetRenderTargets(
        static_cast<UINT>(renderTargetViews.size()),
        renderTargetViews.data(),
        depthStencilView);
}

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
void checkUnbindingRenderTargetsError(
    const std::vector<std::weak_ptr<RenderTarget2D>>& renderTargets,
    const std::vector<std::weak_ptr<Texture>>& textures)
{
    for (auto& renderTarget : renderTargets) {
        for (auto& texture : textures) {
            if (!renderTarget.expired() && !texture.expired()) {
                POMDOG_ASSERT(renderTarget.lock() != texture.lock());
            }
        }
    }
}
#endif

} // namespace

std::unique_ptr<Error>
GraphicsContextDirect3D11::initialize(
    HWND windowHandle,
    const Microsoft::WRL::ComPtr<IDXGIFactory1>& dxgiFactory,
    const Microsoft::WRL::ComPtr<ID3D11Device3>& device,
    const PresentationParameters& presentationParameters) noexcept
{
    POMDOG_ASSERT(device != nullptr);

    blendFactor_ = {{1.0f, 1.0f, 1.0f, 1.0f}};
    preferredBackBufferWidth_ = 1;
    preferredBackBufferHeight_ = 1;
    backBufferCount_ = 2;
    backBufferFormat_ = dxgi::toDXGIFormat(presentationParameters.backBufferFormat);
    backBufferDepthFormat_ = presentationParameters.depthStencilFormat;
    needToApplyPipelineState_ = true;

    DXGI_SAMPLE_DESC sampleDesc;
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    if (presentationParameters.multiSampleCount > 1) {
        chooseMultiSampleSetting(
            device.Get(),
            backBufferFormat_,
            presentationParameters.multiSampleCount,
            sampleDesc);
    }

    {
        device->GetImmediateContext3(&immediateContext_);
        POMDOG_ASSERT(immediateContext_);

        auto hr = device->CreateDeferredContext3(0, &deferredContext_);
        if (FAILED(hr)) {
            return errors::make("CreateDeferredContext3() failed");
        }
        POMDOG_ASSERT(deferredContext_);
    }
    {
        RECT rect;
        ::GetClientRect(windowHandle, &rect);
        auto const windowWidth = rect.right - rect.left;
        auto const windowHeight = rect.bottom - rect.top;

        preferredBackBufferWidth_ = std::max<int>(preferredBackBufferWidth_, windowWidth);
        preferredBackBufferHeight_ = std::max<int>(preferredBackBufferHeight_, windowHeight);
    }
    {
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
        swapChainDesc.BufferCount = backBufferCount_;
        swapChainDesc.BufferDesc.Width = preferredBackBufferWidth_;
        swapChainDesc.BufferDesc.Height = preferredBackBufferHeight_;
        swapChainDesc.BufferDesc.Format = backBufferFormat_;
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = windowHandle;
        swapChainDesc.Windowed = (!presentationParameters.isFullScreen ? TRUE : FALSE);
        swapChainDesc.SampleDesc.Count = sampleDesc.Count;
        swapChainDesc.SampleDesc.Quality = sampleDesc.Quality;

        POMDOG_ASSERT(dxgiFactory);
        HRESULT hr = dxgiFactory->CreateSwapChain(device.Get(), &swapChainDesc, &swapChain_);

        if (FAILED(hr)) {
            return errors::make("CreateSwapChain() failed");
        }
    }
    {
        // TODO: MSAA is not implemented yet
        constexpr int multiSampleCount = 1;

        constexpr std::int32_t backBufferMipLevels = 1;

        backBuffer_ = std::make_shared<RenderTarget2DDirect3D11>();
        if (auto err = backBuffer_->initialize(
                device.Get(),
                swapChain_.Get(),
                preferredBackBufferWidth_,
                preferredBackBufferHeight_,
                backBufferMipLevels,
                presentationParameters.backBufferFormat,
                multiSampleCount);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to initialize back buffer");
        }

        renderTargets_.reserve(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
        renderTargets_.push_back(backBuffer_);
    }

    if (backBufferDepthFormat_ != PixelFormat::Invalid) {
        // TODO: MSAA is not implemented yet.
        constexpr int multiSampleCount = 1;

        backBufferDepthStencil_ = std::make_shared<DepthStencilBufferDirect3D11>();
        if (auto err = backBufferDepthStencil_->initialize(
                device.Get(),
                preferredBackBufferWidth_,
                preferredBackBufferHeight_,
                backBufferDepthFormat_,
                multiSampleCount);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to initialize depth stencil buffer");
        }
    }

    textureResourceViews_.fill(nullptr);

    // NOTE: Set default values for graphics context
    setBlendFactor(Vector4{1.0f, 1.0f, 1.0f, 1.0f});

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    auto graphicsCapbilities = getCapabilities();

    POMDOG_ASSERT(graphicsCapbilities.SamplerSlotCount > 0);
    weakTextures_.resize(graphicsCapbilities.SamplerSlotCount);
#endif
    return nullptr;
}

GraphicsContextDirect3D11::~GraphicsContextDirect3D11()
{
    textureResourceViews_.fill(nullptr);
    renderTargets_.clear();
    backBuffer_.reset();
    swapChain_.Reset();
    deferredContext_.Reset();
    immediateContext_.Reset();
}

void GraphicsContextDirect3D11::executeCommandLists(
    std::span<std::shared_ptr<CommandListImmediate>> commandLists)
{
    pipelineState_ = nullptr;
    needToApplyPipelineState_ = true;

    for (auto& commandList : commandLists) {
        POMDOG_ASSERT(commandList);
        commandList->executeImmediate(*this);
    }

    ComPtr<ID3D11CommandList> nativeCommandList;
    deferredContext_->FinishCommandList(false, &nativeCommandList);
    immediateContext_->ExecuteCommandList(nativeCommandList.Get(), false);
}

void GraphicsContextDirect3D11::present()
{
    POMDOG_ASSERT(swapChain_);
    swapChain_->Present(0, 0);
}

void GraphicsContextDirect3D11::applyPipelineState()
{
    POMDOG_ASSERT(pipelineState_);
    POMDOG_ASSERT(deferredContext_);

    if (needToApplyPipelineState_) {
        pipelineState_->apply(deferredContext_.Get(), blendFactor_.data());
        needToApplyPipelineState_ = false;
    }
}

void GraphicsContextDirect3D11::draw(
    std::uint32_t vertexCount,
    std::uint32_t startVertexLocation)
{
    POMDOG_ASSERT(deferredContext_);
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    checkUnbindingRenderTargetsError(weakRenderTargets_, weakTextures_);
#endif

    applyPipelineState();

    deferredContext_->Draw(static_cast<UINT>(vertexCount), static_cast<UINT>(startVertexLocation));
}

void GraphicsContextDirect3D11::drawIndexed(
    std::uint32_t indexCount,
    std::uint32_t startIndexLocation)
{
    POMDOG_ASSERT(deferredContext_);
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    checkUnbindingRenderTargetsError(weakRenderTargets_, weakTextures_);
#endif

    applyPipelineState();

    deferredContext_->DrawIndexed(static_cast<UINT>(indexCount), static_cast<UINT>(startIndexLocation), 0);
}

void GraphicsContextDirect3D11::drawInstanced(
    std::uint32_t vertexCountPerInstance,
    std::uint32_t instanceCount,
    std::uint32_t startVertexLocation,
    std::uint32_t startInstanceLocation)
{
    POMDOG_ASSERT(deferredContext_);
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    checkUnbindingRenderTargetsError(weakRenderTargets_, weakTextures_);
#endif

    applyPipelineState();

    deferredContext_->DrawInstanced(
        static_cast<UINT>(vertexCountPerInstance),
        static_cast<UINT>(instanceCount),
        static_cast<UINT>(startVertexLocation),
        static_cast<UINT>(startInstanceLocation));
}

void GraphicsContextDirect3D11::drawIndexedInstanced(
    std::uint32_t indexCountPerInstance,
    std::uint32_t instanceCount,
    std::uint32_t startIndexLocation,
    std::uint32_t startInstanceLocation)
{
    POMDOG_ASSERT(deferredContext_);
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    checkUnbindingRenderTargetsError(weakRenderTargets_, weakTextures_);
#endif

    applyPipelineState();

    deferredContext_->DrawIndexedInstanced(
        static_cast<UINT>(indexCountPerInstance),
        static_cast<UINT>(instanceCount),
        static_cast<UINT>(startIndexLocation),
        0,
        static_cast<UINT>(startInstanceLocation));
}

void GraphicsContextDirect3D11::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    POMDOG_ASSERT(deferredContext_ != nullptr);
    POMDOG_ASSERT(indexBuffer != nullptr);

    const auto nativeBuffer = static_cast<BufferDirect3D11*>(indexBuffer->getBuffer());
    POMDOG_ASSERT(nativeBuffer != nullptr);
    POMDOG_ASSERT(nativeBuffer == dynamic_cast<BufferDirect3D11*>(indexBuffer->getBuffer()));

    deferredContext_->IASetIndexBuffer(
        nativeBuffer->getBuffer(),
        dxgi::toDXGIFormat(indexBuffer->getElementSize()),
        0);
}

GraphicsCapabilities
GraphicsContextDirect3D11::getCapabilities() const noexcept
{
    GraphicsCapabilities caps;
    caps.SamplerSlotCount = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT;
    caps.ConstantBufferSlotCount = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
    return caps;
}

void GraphicsContextDirect3D11::setViewport(const Viewport& viewportIn)
{
    POMDOG_ASSERT(0 < viewportIn.width);
    POMDOG_ASSERT(0 < viewportIn.height);
    POMDOG_ASSERT(D3D11_VIEWPORT_BOUNDS_MAX >= viewportIn.topLeftX + viewportIn.width);
    POMDOG_ASSERT(D3D11_VIEWPORT_BOUNDS_MAX >= viewportIn.topLeftY + viewportIn.height);

    // NOTE: The MinDepth and MaxDepth must be between 0 and 1, respectively.
    // Please see https://msdn.microsoft.com/en-us/library/windows/desktop/ff476260(v=vs.85).aspx
    POMDOG_ASSERT((0.0f <= viewportIn.minDepth) && (viewportIn.minDepth <= 1.0f));
    POMDOG_ASSERT((0.0f <= viewportIn.maxDepth) && (viewportIn.maxDepth <= 1.0f));

    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<FLOAT>(viewportIn.width);
    viewport.Height = static_cast<FLOAT>(viewportIn.height);
    viewport.MinDepth = viewportIn.minDepth;
    viewport.MaxDepth = viewportIn.maxDepth;
    viewport.TopLeftX = static_cast<FLOAT>(viewportIn.topLeftX);
    viewport.TopLeftY = static_cast<FLOAT>(viewportIn.topLeftY);

    POMDOG_ASSERT(deferredContext_);
    deferredContext_->RSSetViewports(1, &viewport);
}

void GraphicsContextDirect3D11::setScissorRect(const Rectangle& scissorRect)
{
    D3D11_RECT rect;
    rect.left = scissorRect.getLeft();
    rect.top = scissorRect.getTop();
    rect.right = scissorRect.getRight();
    rect.bottom = scissorRect.getBottom();

    POMDOG_ASSERT(deferredContext_ != nullptr);
    deferredContext_->RSSetScissorRects(1, &rect);
}

void GraphicsContextDirect3D11::setBlendFactor(const Vector4& blendFactorIn)
{
    blendFactor_[0] = blendFactorIn.x;
    blendFactor_[1] = blendFactorIn.y;
    blendFactor_[2] = blendFactorIn.z;
    blendFactor_[3] = blendFactorIn.w;
    needToApplyPipelineState_ = true;
}

void GraphicsContextDirect3D11::setVertexBuffer(
    std::uint32_t index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer,
    std::uint32_t offset)
{
    POMDOG_ASSERT(vertexBuffer != nullptr);
    POMDOG_ASSERT(vertexBuffer->getBuffer() != nullptr);

    auto nativeBuffer = static_cast<BufferDirect3D11*>(vertexBuffer->getBuffer());
    POMDOG_ASSERT(nativeBuffer != nullptr);
    POMDOG_ASSERT(nativeBuffer == dynamic_cast<BufferDirect3D11*>(vertexBuffer->getBuffer()));

    const auto buffer = nativeBuffer->getBuffer();
    const auto stride = static_cast<UINT>(vertexBuffer->getStrideBytes());
    const auto vertexOffset = static_cast<UINT>(offset);

    POMDOG_ASSERT(deferredContext_ != nullptr);
    deferredContext_->IASetVertexBuffers(
        static_cast<UINT>(index),
        1,
        &buffer,
        &stride,
        &vertexOffset);
}

void GraphicsContextDirect3D11::setPipelineState(const std::shared_ptr<PipelineState>& pipelineStateIn)
{
    POMDOG_ASSERT(pipelineStateIn);

    if (pipelineState_ != pipelineStateIn) {
        pipelineState_ = std::dynamic_pointer_cast<PipelineStateDirect3D11>(pipelineStateIn);
        POMDOG_ASSERT(pipelineState_);

        needToApplyPipelineState_ = true;
    }
}

void GraphicsContextDirect3D11::setConstantBuffer(
    std::uint32_t index,
    const std::shared_ptr<Buffer>& constantBufferIn,
    std::uint32_t offset,
    std::uint32_t sizeInBytes)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);

    const auto constantBuffer = static_cast<BufferDirect3D11*>(constantBufferIn.get());
    POMDOG_ASSERT(constantBuffer != nullptr);
    POMDOG_ASSERT(constantBuffer == dynamic_cast<BufferDirect3D11*>(constantBufferIn.get()));

    const auto buffer = constantBuffer->getBuffer();
    POMDOG_ASSERT(buffer != nullptr);

    const auto startOffset = static_cast<UINT>(offset);
    const auto constantSize = static_cast<UINT>(sizeInBytes);

    POMDOG_ASSERT(deferredContext_);
    deferredContext_->VSSetConstantBuffers1(index, 1, &buffer, &startOffset, &constantSize);
    deferredContext_->PSSetConstantBuffers1(index, 1, &buffer, &startOffset, &constantSize);
}

void GraphicsContextDirect3D11::setSampler(std::uint32_t index, const std::shared_ptr<SamplerState>& samplerIn)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
    POMDOG_ASSERT(samplerIn != nullptr);

    const auto sampler = std::static_pointer_cast<SamplerStateDirect3D11>(samplerIn);

    POMDOG_ASSERT(sampler != nullptr);
    POMDOG_ASSERT(sampler == std::dynamic_pointer_cast<SamplerStateDirect3D11>(samplerIn));
    POMDOG_ASSERT(sampler->getSamplerState() != nullptr);

    std::array<ID3D11SamplerState*, 1> const states = {
        sampler->getSamplerState(),
    };

    POMDOG_ASSERT(deferredContext_);
    deferredContext_->PSSetSamplers(index, static_cast<UINT>(states.size()), states.data());
}

void GraphicsContextDirect3D11::setTexture(std::uint32_t index)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(textureResourceViews_.size()));

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures_.empty());
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(weakTextures_.size()));
    weakTextures_[index].reset();
#endif

    textureResourceViews_[index] = nullptr;

    POMDOG_ASSERT(deferredContext_);
    deferredContext_->PSSetShaderResources(index, 1, &textureResourceViews_[index]);
}

void GraphicsContextDirect3D11::setTexture(std::uint32_t index, const std::shared_ptr<gpu::Texture2D>& textureIn)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(textureResourceViews_.size()));
    POMDOG_ASSERT(textureIn != nullptr);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures_.empty());
    POMDOG_ASSERT(index < static_cast<int>(weakTextures_.size()));
    weakTextures_[index] = textureIn;
#endif

    const auto texture = static_cast<Texture2DDirect3D11*>(textureIn.get());

    POMDOG_ASSERT(texture != nullptr);
    POMDOG_ASSERT(texture == dynamic_cast<Texture2DDirect3D11*>(textureIn.get()));

    textureResourceViews_[index] = texture->getShaderResourceView();

    POMDOG_ASSERT(deferredContext_);
    deferredContext_->PSSetShaderResources(index, 1, &textureResourceViews_[index]);
}

void GraphicsContextDirect3D11::setTexture(std::uint32_t index, const std::shared_ptr<RenderTarget2D>& textureIn)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(textureResourceViews_.size()));
    POMDOG_ASSERT(textureIn != nullptr);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures_.empty());
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(weakTextures_.size()));
    weakTextures_[index] = textureIn;
#endif

    const auto texture = static_cast<RenderTarget2DDirect3D11*>(textureIn.get());

    POMDOG_ASSERT(texture != nullptr);
    POMDOG_ASSERT(texture == dynamic_cast<RenderTarget2DDirect3D11*>(textureIn.get()));

    textureResourceViews_[index] = texture->getShaderResourceView();

    POMDOG_ASSERT(deferredContext_);
    deferredContext_->PSSetShaderResources(index, 1, &textureResourceViews_[index]);
}

void GraphicsContextDirect3D11::beginRenderPass(const RenderPass& renderPass)
{
    POMDOG_ASSERT(deferredContext_);
    POMDOG_ASSERT(!renderPass.renderTargets.empty());
    POMDOG_ASSERT(renderPass.renderTargets.size() == 8);

    const bool useBackBuffer = (std::get<0>(renderPass.renderTargets.front()) == nullptr);

    if (useBackBuffer) {
        useBackBufferAsRenderTarget(deferredContext_, renderTargets_, backBuffer_, backBufferDepthStencil_);
    }
    else {
        auto& renderTargetsIn = renderPass.renderTargets;
        POMDOG_ASSERT(!renderTargetsIn.empty());
        POMDOG_ASSERT(renderTargetsIn.size() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
        POMDOG_ASSERT(renderTargetsIn.size() <= renderTargets_.capacity());

        renderTargets_.clear();
        std::array<ID3D11RenderTargetView*, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> renderTargetViews;

        POMDOG_ASSERT(renderTargetViews.size() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
        POMDOG_ASSERT(renderTargetViews.size() >= renderTargetsIn.size());

        for (std::size_t i = 0; i < renderTargetsIn.size(); ++i) {
            auto& renderTarget = std::get<0>(renderTargetsIn[i]);
            if (renderTarget == nullptr) {
                break;
            }
            auto direct3d11RenderTarget = std::static_pointer_cast<RenderTarget2DDirect3D11>(renderTarget);
            POMDOG_ASSERT(direct3d11RenderTarget != nullptr);
            POMDOG_ASSERT(direct3d11RenderTarget == std::dynamic_pointer_cast<RenderTarget2DDirect3D11>(renderTarget));

            renderTargetViews[i] = direct3d11RenderTarget->getRenderTargetView();
            renderTargets_.emplace_back(std::move(direct3d11RenderTarget));
            POMDOG_ASSERT(renderTargetViews[i] != nullptr);
            POMDOG_ASSERT(i <= renderTargets_.size());
        }

        ID3D11DepthStencilView* depthStencilView = nullptr;
        if (const auto& p = renderPass.depthStencilBuffer; p != nullptr) {
            auto nativeBuffer = std::static_pointer_cast<DepthStencilBufferDirect3D11>(p);
            POMDOG_ASSERT(nativeBuffer != nullptr);
            POMDOG_ASSERT(nativeBuffer == std::dynamic_pointer_cast<DepthStencilBufferDirect3D11>(p));
            depthStencilView = nativeBuffer->getDepthStencilView();
        }

        deferredContext_->OMSetRenderTargets(
            static_cast<UINT>(renderTargets_.size()),
            renderTargetViews.data(),
            depthStencilView);
    }

    if (renderPass.viewport) {
        auto& viewportIn = *renderPass.viewport;
        POMDOG_ASSERT(0 < viewportIn.width);
        POMDOG_ASSERT(0 < viewportIn.height);
        POMDOG_ASSERT(D3D11_VIEWPORT_BOUNDS_MAX >= viewportIn.topLeftX + viewportIn.width);
        POMDOG_ASSERT(D3D11_VIEWPORT_BOUNDS_MAX >= viewportIn.topLeftY + viewportIn.height);

        // NOTE: The MinDepth and MaxDepth must be between 0 and 1, respectively.
        // Please see https://msdn.microsoft.com/en-us/library/windows/desktop/ff476260(v=vs.85).aspx
        POMDOG_ASSERT((0.0f <= viewportIn.minDepth) && (viewportIn.minDepth <= 1.0f));
        POMDOG_ASSERT((0.0f <= viewportIn.maxDepth) && (viewportIn.maxDepth <= 1.0f));

        D3D11_VIEWPORT viewport;
        viewport.Width = static_cast<FLOAT>(viewportIn.width);
        viewport.Height = static_cast<FLOAT>(viewportIn.height);
        viewport.MinDepth = viewportIn.minDepth;
        viewport.MaxDepth = viewportIn.maxDepth;
        viewport.TopLeftX = static_cast<FLOAT>(viewportIn.topLeftX);
        viewport.TopLeftY = static_cast<FLOAT>(viewportIn.topLeftY);

        POMDOG_ASSERT(deferredContext_);
        deferredContext_->RSSetViewports(1, &viewport);
    }
    else if (useBackBuffer) {
        POMDOG_ASSERT(!renderPass.viewport);

        D3D11_VIEWPORT viewport;
        viewport.Width = static_cast<FLOAT>(preferredBackBufferWidth_);
        viewport.Height = static_cast<FLOAT>(preferredBackBufferHeight_);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;

        POMDOG_ASSERT(deferredContext_);
        deferredContext_->RSSetViewports(1, &viewport);
    }

    if (renderPass.scissorRect) {
        auto& scissorRectIn = *renderPass.scissorRect;

        D3D11_RECT rect;
        rect.left = scissorRectIn.getLeft();
        rect.top = scissorRectIn.getTop();
        rect.right = scissorRectIn.getRight();
        rect.bottom = scissorRectIn.getBottom();

        POMDOG_ASSERT(deferredContext_);
        deferredContext_->RSSetScissorRects(1, &rect);
    }
    else if (useBackBuffer) {
        POMDOG_ASSERT(!renderPass.scissorRect);

        D3D11_RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = preferredBackBufferWidth_;
        rect.bottom = preferredBackBufferHeight_;

        POMDOG_ASSERT(deferredContext_);
        deferredContext_->RSSetScissorRects(1, &rect);
    }

    POMDOG_ASSERT(renderTargets_.size() <= renderPass.renderTargets.size());
    for (std::size_t i = 0; i < renderTargets_.size(); ++i) {
        auto& clearColor = std::get<1>(renderPass.renderTargets[i]);
        if (clearColor) {
            auto& renderTarget = renderTargets_[i];
            POMDOG_ASSERT(renderTarget);

            deferredContext_->ClearRenderTargetView(
                renderTarget->getRenderTargetView(), clearColor->data());
        }
    }

    {
        FLOAT depth = 1.0f;
        UINT8 stencil = 0;
        UINT mask = 0;

        if (renderPass.clearDepth) {
            depth = *renderPass.clearDepth;
            mask |= D3D11_CLEAR_DEPTH;
        }
        if (renderPass.clearStencil) {
            stencil = *renderPass.clearStencil;
            mask |= D3D11_CLEAR_STENCIL;
            POMDOG_ASSERT(stencil >= 0);
            POMDOG_ASSERT(stencil <= std::numeric_limits<UINT8>::max());
        }

        if (mask != 0) {
            ID3D11DepthStencilView* depthStencilView = nullptr;
            if (useBackBuffer) {
                if (backBufferDepthStencil_ != nullptr) {
                    depthStencilView = backBufferDepthStencil_->getDepthStencilView();
                }
            }
            else if (const auto& p = renderPass.depthStencilBuffer; p != nullptr) {
                auto nativeBuffer = std::static_pointer_cast<DepthStencilBufferDirect3D11>(p);
                POMDOG_ASSERT(nativeBuffer != nullptr);
                POMDOG_ASSERT(nativeBuffer == std::dynamic_pointer_cast<DepthStencilBufferDirect3D11>(p));
                depthStencilView = nativeBuffer->getDepthStencilView();
            }

            if (depthStencilView != nullptr) {
                deferredContext_->ClearDepthStencilView(depthStencilView, mask, depth, stencil);
            }
        }
    }
}

void GraphicsContextDirect3D11::endRenderPass()
{
}

std::unique_ptr<Error>
GraphicsContextDirect3D11::resizeBackBuffers(
    ID3D11Device* device, int backBufferWidthIn, int backBufferHeightIn) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(backBufferWidthIn > 0);
    POMDOG_ASSERT(backBufferHeightIn > 0);

    preferredBackBufferWidth_ = backBufferWidthIn;
    preferredBackBufferHeight_ = backBufferHeightIn;

    POMDOG_ASSERT(backBuffer_ != nullptr);
    backBuffer_->resetBackBuffer();

    POMDOG_ASSERT(swapChain_ != nullptr);
    if (auto hr = swapChain_->ResizeBuffers(
            backBufferCount_,
            preferredBackBufferWidth_,
            preferredBackBufferHeight_,
            backBufferFormat_,
            0);
        FAILED(hr)) {
        return errors::make("failed to resize back buffer");
    }

    if (auto err = backBuffer_->resetBackBuffer(
            device,
            swapChain_.Get(),
            preferredBackBufferWidth_,
            preferredBackBufferHeight_);
        err != nullptr) {
        return errors::wrap(std::move(err), "backBuffer->resetBackBuffer() failed");
    }

    if (backBufferDepthStencil_ != nullptr) {
        // TODO: MSAA
        constexpr std::int32_t multiSampleCount = 1;

        if (auto err = backBufferDepthStencil_->resetBuffer(
                device,
                preferredBackBufferWidth_,
                preferredBackBufferHeight_,
                backBufferDepthFormat_,
                multiSampleCount);
            err != nullptr) {
            return errors::wrap(std::move(err), "backBufferDepthStencil->resetBuffer() failed");
        }
    }

    return nullptr;
}

ID3D11DeviceContext3*
GraphicsContextDirect3D11::getImmediateContext() noexcept
{
    POMDOG_ASSERT(immediateContext_);
    return immediateContext_.Get();
}

ID3D11DeviceContext3*
GraphicsContextDirect3D11::getDeferredContext() noexcept
{
    POMDOG_ASSERT(deferredContext_);
    return deferredContext_.Get();
}

} // namespace pomdog::gpu::detail::direct3d11
