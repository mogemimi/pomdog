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

void ChooseMultiSampleSetting(
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

void UseBackBufferAsRenderTarget(
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
        backBuffer->GetRenderTargetView()};

    ID3D11DepthStencilView* depthStencilView = nullptr;
    if (depthStencilBuffer != nullptr) {
        depthStencilView = depthStencilBuffer->GetDepthStencilView();
    }

    deferredContext->OMSetRenderTargets(
        static_cast<UINT>(renderTargetViews.size()),
        renderTargetViews.data(),
        depthStencilView);
}

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
void CheckUnbindingRenderTargetsError(
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
GraphicsContextDirect3D11::Initialize(
    HWND windowHandle,
    const Microsoft::WRL::ComPtr<IDXGIFactory1>& dxgiFactory,
    const Microsoft::WRL::ComPtr<ID3D11Device3>& device,
    const PresentationParameters& presentationParameters) noexcept
{
    POMDOG_ASSERT(device != nullptr);

    blendFactor = {1.0f, 1.0f, 1.0f, 1.0f};
    preferredBackBufferWidth = 1;
    preferredBackBufferHeight = 1;
    backBufferCount = 2;
    backBufferFormat = dxgi::ToDXGIFormat(presentationParameters.BackBufferFormat);
    backBufferDepthFormat = presentationParameters.DepthStencilFormat;
    needToApplyPipelineState = true;

    DXGI_SAMPLE_DESC sampleDesc;
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    if (presentationParameters.MultiSampleCount > 1) {
        ChooseMultiSampleSetting(
            device.Get(),
            backBufferFormat,
            presentationParameters.MultiSampleCount,
            sampleDesc);
    }

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    Log::Internal(StringHelper::Format(
        "DXGI_SAMPLE_DESC.Count  : %d\n"
        "DXGI_SAMPLE_DESC.Quality: %d",
        sampleDesc.Count,
        sampleDesc.Quality));
#endif

    {
        device->GetImmediateContext3(&immediateContext);
        POMDOG_ASSERT(immediateContext);

        auto hr = device->CreateDeferredContext3(0, &deferredContext);
        if (FAILED(hr)) {
            return errors::New("CreateDeferredContext3() failed");
        }
        POMDOG_ASSERT(deferredContext);
    }
    {
        RECT rect;
        ::GetClientRect(windowHandle, &rect);
        auto const windowWidth = rect.right - rect.left;
        auto const windowHeight = rect.bottom - rect.top;

        preferredBackBufferWidth = std::max<int>(preferredBackBufferWidth, windowWidth);
        preferredBackBufferHeight = std::max<int>(preferredBackBufferHeight, windowHeight);
    }
    {
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
        swapChainDesc.BufferCount = backBufferCount;
        swapChainDesc.BufferDesc.Width = preferredBackBufferWidth;
        swapChainDesc.BufferDesc.Height = preferredBackBufferHeight;
        swapChainDesc.BufferDesc.Format = backBufferFormat;
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = windowHandle;
        swapChainDesc.Windowed = (!presentationParameters.IsFullScreen ? TRUE : FALSE);
        swapChainDesc.SampleDesc.Count = sampleDesc.Count;
        swapChainDesc.SampleDesc.Quality = sampleDesc.Quality;

        POMDOG_ASSERT(dxgiFactory);
        HRESULT hr = dxgiFactory->CreateSwapChain(device.Get(), &swapChainDesc, &swapChain);

        if (FAILED(hr)) {
            return errors::New("CreateSwapChain() failed");
        }
    }
    {
        // TODO: MSAA is not implemented yet
        constexpr int multiSampleCount = 1;

        constexpr std::int32_t backBufferMipLevels = 1;

        backBuffer = std::make_shared<RenderTarget2DDirect3D11>();
        if (auto err = backBuffer->Initialize(
                device.Get(),
                swapChain.Get(),
                preferredBackBufferWidth,
                preferredBackBufferHeight,
                backBufferMipLevels,
                presentationParameters.BackBufferFormat,
                multiSampleCount);
            err != nullptr) {
            return errors::Wrap(std::move(err), "failed to initialize back buffer");
        }

        renderTargets.reserve(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
        renderTargets.push_back(backBuffer);
    }

    if (backBufferDepthFormat != PixelFormat::Invalid) {
        // TODO: MSAA is not implemented yet.
        constexpr int multiSampleCount = 1;

        backBufferDepthStencil = std::make_shared<DepthStencilBufferDirect3D11>();
        if (auto err = backBufferDepthStencil->Initialize(
                device.Get(),
                preferredBackBufferWidth,
                preferredBackBufferHeight,
                backBufferDepthFormat,
                multiSampleCount);
            err != nullptr) {
            return errors::Wrap(std::move(err), "failed to initialize depth stencil buffer");
        }
    }

    textureResourceViews.fill(nullptr);

    // NOTE: Set default values for graphics context
    this->SetBlendFactor(Vector4{1.0f, 1.0f, 1.0f, 1.0f});

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    auto graphicsCapbilities = this->GetCapabilities();

    POMDOG_ASSERT(graphicsCapbilities.SamplerSlotCount > 0);
    weakTextures.resize(graphicsCapbilities.SamplerSlotCount);
#endif
    return nullptr;
}

GraphicsContextDirect3D11::~GraphicsContextDirect3D11()
{
    textureResourceViews.fill(nullptr);
    renderTargets.clear();
    backBuffer.reset();
    swapChain.Reset();
    deferredContext.Reset();
    immediateContext.Reset();
}

void GraphicsContextDirect3D11::ExecuteCommandLists(
    std::span<std::shared_ptr<CommandListImmediate>> commandLists)
{
    pipelineState = nullptr;
    needToApplyPipelineState = true;

    for (auto& commandList : commandLists) {
        POMDOG_ASSERT(commandList);
        commandList->ExecuteImmediate(*this);
    }

    ComPtr<ID3D11CommandList> nativeCommandList;
    deferredContext->FinishCommandList(false, &nativeCommandList);
    immediateContext->ExecuteCommandList(nativeCommandList.Get(), false);
}

void GraphicsContextDirect3D11::Present()
{
    POMDOG_ASSERT(swapChain);
    swapChain->Present(0, 0);
}

void GraphicsContextDirect3D11::ApplyPipelineState()
{
    POMDOG_ASSERT(pipelineState);
    POMDOG_ASSERT(deferredContext);

    if (needToApplyPipelineState) {
        pipelineState->Apply(deferredContext.Get(), blendFactor.data());
        needToApplyPipelineState = false;
    }
}

void GraphicsContextDirect3D11::Draw(
    std::uint32_t vertexCount,
    std::uint32_t startVertexLocation)
{
    POMDOG_ASSERT(deferredContext);
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets, weakTextures);
#endif

    ApplyPipelineState();

    deferredContext->Draw(static_cast<UINT>(vertexCount), static_cast<UINT>(startVertexLocation));
}

void GraphicsContextDirect3D11::DrawIndexed(
    std::uint32_t indexCount,
    std::uint32_t startIndexLocation)
{
    POMDOG_ASSERT(deferredContext);
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets, weakTextures);
#endif

    ApplyPipelineState();

    deferredContext->DrawIndexed(static_cast<UINT>(indexCount), static_cast<UINT>(startIndexLocation), 0);
}

void GraphicsContextDirect3D11::DrawInstanced(
    std::uint32_t vertexCountPerInstance,
    std::uint32_t instanceCount,
    std::uint32_t startVertexLocation,
    std::uint32_t startInstanceLocation)
{
    POMDOG_ASSERT(deferredContext);
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets, weakTextures);
#endif

    ApplyPipelineState();

    deferredContext->DrawInstanced(
        static_cast<UINT>(vertexCountPerInstance),
        static_cast<UINT>(instanceCount),
        static_cast<UINT>(startVertexLocation),
        static_cast<UINT>(startInstanceLocation));
}

void GraphicsContextDirect3D11::DrawIndexedInstanced(
    std::uint32_t indexCountPerInstance,
    std::uint32_t instanceCount,
    std::uint32_t startIndexLocation,
    std::uint32_t startInstanceLocation)
{
    POMDOG_ASSERT(deferredContext);
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets, weakTextures);
#endif

    ApplyPipelineState();

    deferredContext->DrawIndexedInstanced(
        static_cast<UINT>(indexCountPerInstance),
        static_cast<UINT>(instanceCount),
        static_cast<UINT>(startIndexLocation),
        0,
        static_cast<UINT>(startInstanceLocation));
}

void GraphicsContextDirect3D11::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    POMDOG_ASSERT(deferredContext != nullptr);
    POMDOG_ASSERT(indexBuffer != nullptr);

    auto nativeBuffer = static_cast<BufferDirect3D11*>(indexBuffer->GetBuffer());
    POMDOG_ASSERT(nativeBuffer != nullptr);
    POMDOG_ASSERT(nativeBuffer == dynamic_cast<BufferDirect3D11*>(indexBuffer->GetBuffer()));

    deferredContext->IASetIndexBuffer(
        nativeBuffer->GetBuffer(),
        dxgi::ToDXGIFormat(indexBuffer->GetElementSize()),
        0);
}

GraphicsCapabilities GraphicsContextDirect3D11::GetCapabilities() const noexcept
{
    GraphicsCapabilities caps;
    caps.SamplerSlotCount = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT;
    caps.ConstantBufferSlotCount = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
    return caps;
}

void GraphicsContextDirect3D11::SetViewport(const Viewport& viewportIn)
{
    POMDOG_ASSERT(0 < viewportIn.Width);
    POMDOG_ASSERT(0 < viewportIn.Height);
    POMDOG_ASSERT(D3D11_VIEWPORT_BOUNDS_MAX >= viewportIn.TopLeftX + viewportIn.Width);
    POMDOG_ASSERT(D3D11_VIEWPORT_BOUNDS_MAX >= viewportIn.TopLeftY + viewportIn.Height);

    // NOTE: The MinDepth and MaxDepth must be between 0 and 1, respectively.
    // Please see https://msdn.microsoft.com/en-us/library/windows/desktop/ff476260(v=vs.85).aspx
    POMDOG_ASSERT((0.0f <= viewportIn.MinDepth) && (viewportIn.MinDepth <= 1.0f));
    POMDOG_ASSERT((0.0f <= viewportIn.MaxDepth) && (viewportIn.MaxDepth <= 1.0f));

    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<FLOAT>(viewportIn.Width);
    viewport.Height = static_cast<FLOAT>(viewportIn.Height);
    viewport.MinDepth = viewportIn.MinDepth;
    viewport.MaxDepth = viewportIn.MaxDepth;
    viewport.TopLeftX = static_cast<FLOAT>(viewportIn.TopLeftX);
    viewport.TopLeftY = static_cast<FLOAT>(viewportIn.TopLeftY);

    POMDOG_ASSERT(deferredContext);
    deferredContext->RSSetViewports(1, &viewport);
}

void GraphicsContextDirect3D11::SetScissorRect(const Rectangle& scissorRect)
{
    D3D11_RECT rect;
    rect.left = scissorRect.GetLeft();
    rect.top = scissorRect.GetTop();
    rect.right = scissorRect.GetRight();
    rect.bottom = scissorRect.GetBottom();

    POMDOG_ASSERT(deferredContext != nullptr);
    deferredContext->RSSetScissorRects(1, &rect);
}

void GraphicsContextDirect3D11::SetBlendFactor(const Vector4& blendFactorIn)
{
    blendFactor[0] = blendFactorIn.X;
    blendFactor[1] = blendFactorIn.Y;
    blendFactor[2] = blendFactorIn.Z;
    blendFactor[3] = blendFactorIn.W;
    needToApplyPipelineState = true;
}

void GraphicsContextDirect3D11::SetVertexBuffer(
    std::uint32_t index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer,
    std::uint32_t offset)
{
    POMDOG_ASSERT(vertexBuffer != nullptr);
    POMDOG_ASSERT(vertexBuffer->GetBuffer() != nullptr);

    auto nativeBuffer = static_cast<BufferDirect3D11*>(vertexBuffer->GetBuffer());
    POMDOG_ASSERT(nativeBuffer != nullptr);
    POMDOG_ASSERT(nativeBuffer == dynamic_cast<BufferDirect3D11*>(vertexBuffer->GetBuffer()));

    const auto buffer = nativeBuffer->GetBuffer();
    const auto stride = static_cast<UINT>(vertexBuffer->GetStrideBytes());
    const auto vertexOffset = static_cast<UINT>(offset);

    POMDOG_ASSERT(deferredContext != nullptr);
    deferredContext->IASetVertexBuffers(
        static_cast<UINT>(index),
        1,
        &buffer,
        &stride,
        &vertexOffset);
}

void GraphicsContextDirect3D11::SetPipelineState(const std::shared_ptr<PipelineState>& pipelineStateIn)
{
    POMDOG_ASSERT(pipelineStateIn);

    if (pipelineState != pipelineStateIn) {
        pipelineState = std::dynamic_pointer_cast<PipelineStateDirect3D11>(pipelineStateIn);
        POMDOG_ASSERT(pipelineState);

        needToApplyPipelineState = true;
    }
}

void GraphicsContextDirect3D11::SetConstantBuffer(
    std::uint32_t index,
    const std::shared_ptr<Buffer>& constantBufferIn,
    std::uint32_t offset,
    std::uint32_t sizeInBytes)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);

    auto constantBuffer = static_cast<BufferDirect3D11*>(constantBufferIn.get());
    POMDOG_ASSERT(constantBuffer != nullptr);
    POMDOG_ASSERT(constantBuffer == dynamic_cast<BufferDirect3D11*>(constantBufferIn.get()));

    auto buffer = constantBuffer->GetBuffer();
    POMDOG_ASSERT(buffer != nullptr);

    const auto startOffset = static_cast<UINT>(offset);
    const auto constantSize = static_cast<UINT>(sizeInBytes);

    POMDOG_ASSERT(deferredContext);
    deferredContext->VSSetConstantBuffers1(index, 1, &buffer, &startOffset, &constantSize);
    deferredContext->PSSetConstantBuffers1(index, 1, &buffer, &startOffset, &constantSize);
}

void GraphicsContextDirect3D11::SetSampler(std::uint32_t index, const std::shared_ptr<SamplerState>& samplerIn)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
    POMDOG_ASSERT(samplerIn != nullptr);

    auto sampler = std::static_pointer_cast<SamplerStateDirect3D11>(samplerIn);

    POMDOG_ASSERT(sampler != nullptr);
    POMDOG_ASSERT(sampler == std::dynamic_pointer_cast<SamplerStateDirect3D11>(samplerIn));
    POMDOG_ASSERT(sampler->GetSamplerState() != nullptr);

    std::array<ID3D11SamplerState*, 1> const states = {
        sampler->GetSamplerState()};

    POMDOG_ASSERT(deferredContext);
    deferredContext->PSSetSamplers(index, static_cast<UINT>(states.size()), states.data());
}

void GraphicsContextDirect3D11::SetTexture(std::uint32_t index)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(textureResourceViews.size()));

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures.empty());
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(weakTextures.size()));
    weakTextures[index].reset();
#endif

    textureResourceViews[index] = nullptr;

    POMDOG_ASSERT(deferredContext);
    deferredContext->PSSetShaderResources(index, 1, &textureResourceViews[index]);
}

void GraphicsContextDirect3D11::SetTexture(std::uint32_t index, const std::shared_ptr<gpu::Texture2D>& textureIn)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(textureResourceViews.size()));
    POMDOG_ASSERT(textureIn != nullptr);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures.empty());
    POMDOG_ASSERT(index < static_cast<int>(weakTextures.size()));
    weakTextures[index] = textureIn;
#endif

    auto texture = static_cast<Texture2DDirect3D11*>(textureIn.get());

    POMDOG_ASSERT(texture != nullptr);
    POMDOG_ASSERT(texture == dynamic_cast<Texture2DDirect3D11*>(textureIn.get()));

    textureResourceViews[index] = texture->GetShaderResourceView();

    POMDOG_ASSERT(deferredContext);
    deferredContext->PSSetShaderResources(0, 1, &textureResourceViews[index]);
}

void GraphicsContextDirect3D11::SetTexture(std::uint32_t index, const std::shared_ptr<RenderTarget2D>& textureIn)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(textureResourceViews.size()));
    POMDOG_ASSERT(textureIn != nullptr);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures.empty());
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(weakTextures.size()));
    weakTextures[index] = textureIn;
#endif

    auto texture = static_cast<RenderTarget2DDirect3D11*>(textureIn.get());

    POMDOG_ASSERT(texture != nullptr);
    POMDOG_ASSERT(texture == dynamic_cast<RenderTarget2DDirect3D11*>(textureIn.get()));

    textureResourceViews[index] = texture->GetShaderResourceView();

    POMDOG_ASSERT(deferredContext);
    deferredContext->PSSetShaderResources(0, 1, &textureResourceViews[index]);
}

void GraphicsContextDirect3D11::BeginRenderPass(const RenderPass& renderPass)
{
    POMDOG_ASSERT(deferredContext);
    POMDOG_ASSERT(!renderPass.RenderTargets.empty());
    POMDOG_ASSERT(renderPass.RenderTargets.size() == 8);

    const bool useBackBuffer = (std::get<0>(renderPass.RenderTargets.front()) == nullptr);

    if (useBackBuffer) {
        UseBackBufferAsRenderTarget(deferredContext, renderTargets, backBuffer, backBufferDepthStencil);
    }
    else {
        auto& renderTargetsIn = renderPass.RenderTargets;
        POMDOG_ASSERT(!renderTargetsIn.empty());
        POMDOG_ASSERT(renderTargetsIn.size() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
        POMDOG_ASSERT(renderTargetsIn.size() <= renderTargets.capacity());

        renderTargets.clear();
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

            renderTargetViews[i] = direct3d11RenderTarget->GetRenderTargetView();
            renderTargets.emplace_back(std::move(direct3d11RenderTarget));
            POMDOG_ASSERT(renderTargetViews[i] != nullptr);
            POMDOG_ASSERT(i <= renderTargets.size());
        }

        ID3D11DepthStencilView* depthStencilView = nullptr;
        if (const auto& p = renderPass.DepthStencilBuffer; p != nullptr) {
            auto nativeBuffer = std::static_pointer_cast<DepthStencilBufferDirect3D11>(p);
            POMDOG_ASSERT(nativeBuffer != nullptr);
            POMDOG_ASSERT(nativeBuffer == std::dynamic_pointer_cast<DepthStencilBufferDirect3D11>(p));
            depthStencilView = nativeBuffer->GetDepthStencilView();
        }

        deferredContext->OMSetRenderTargets(
            static_cast<UINT>(renderTargets.size()),
            renderTargetViews.data(),
            depthStencilView);
    }

    if (renderPass.Viewport) {
        auto& viewportIn = *renderPass.Viewport;
        POMDOG_ASSERT(0 < viewportIn.Width);
        POMDOG_ASSERT(0 < viewportIn.Height);
        POMDOG_ASSERT(D3D11_VIEWPORT_BOUNDS_MAX >= viewportIn.TopLeftX + viewportIn.Width);
        POMDOG_ASSERT(D3D11_VIEWPORT_BOUNDS_MAX >= viewportIn.TopLeftY + viewportIn.Height);

        // NOTE: The MinDepth and MaxDepth must be between 0 and 1, respectively.
        // Please see https://msdn.microsoft.com/en-us/library/windows/desktop/ff476260(v=vs.85).aspx
        POMDOG_ASSERT((0.0f <= viewportIn.MinDepth) && (viewportIn.MinDepth <= 1.0f));
        POMDOG_ASSERT((0.0f <= viewportIn.MaxDepth) && (viewportIn.MaxDepth <= 1.0f));

        D3D11_VIEWPORT viewport;
        viewport.Width = static_cast<FLOAT>(viewportIn.Width);
        viewport.Height = static_cast<FLOAT>(viewportIn.Height);
        viewport.MinDepth = viewportIn.MinDepth;
        viewport.MaxDepth = viewportIn.MaxDepth;
        viewport.TopLeftX = static_cast<FLOAT>(viewportIn.TopLeftX);
        viewport.TopLeftY = static_cast<FLOAT>(viewportIn.TopLeftY);

        POMDOG_ASSERT(deferredContext);
        deferredContext->RSSetViewports(1, &viewport);
    }
    else if (useBackBuffer) {
        POMDOG_ASSERT(!renderPass.Viewport);

        D3D11_VIEWPORT viewport;
        viewport.Width = static_cast<FLOAT>(preferredBackBufferWidth);
        viewport.Height = static_cast<FLOAT>(preferredBackBufferHeight);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;

        POMDOG_ASSERT(deferredContext);
        deferredContext->RSSetViewports(1, &viewport);
    }

    if (renderPass.ScissorRect) {
        auto& scissorRectIn = *renderPass.ScissorRect;

        D3D11_RECT rect;
        rect.left = scissorRectIn.GetLeft();
        rect.top = scissorRectIn.GetTop();
        rect.right = scissorRectIn.GetRight();
        rect.bottom = scissorRectIn.GetBottom();

        POMDOG_ASSERT(deferredContext);
        deferredContext->RSSetScissorRects(1, &rect);
    }
    else if (useBackBuffer) {
        POMDOG_ASSERT(!renderPass.ScissorRect);

        D3D11_RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = preferredBackBufferWidth;
        rect.bottom = preferredBackBufferHeight;

        POMDOG_ASSERT(deferredContext);
        deferredContext->RSSetScissorRects(1, &rect);
    }

    POMDOG_ASSERT(renderTargets.size() <= renderPass.RenderTargets.size());
    for (std::size_t i = 0; i < renderTargets.size(); ++i) {
        auto& clearColor = std::get<1>(renderPass.RenderTargets[i]);
        if (clearColor) {
            auto& renderTarget = renderTargets[i];
            POMDOG_ASSERT(renderTarget);

            deferredContext->ClearRenderTargetView(
                renderTarget->GetRenderTargetView(), clearColor->Data());
        }
    }

    {
        FLOAT depth = 1.0f;
        UINT8 stencil = 0;
        UINT mask = 0;

        if (renderPass.ClearDepth) {
            depth = *renderPass.ClearDepth;
            mask |= D3D11_CLEAR_DEPTH;
        }
        if (renderPass.ClearStencil) {
            stencil = *renderPass.ClearStencil;
            mask |= D3D11_CLEAR_STENCIL;
            POMDOG_ASSERT(stencil >= 0);
            POMDOG_ASSERT(stencil <= std::numeric_limits<UINT8>::max());
        }

        if (mask != 0) {
            ID3D11DepthStencilView* depthStencilView = nullptr;
            if (useBackBuffer) {
                if (backBufferDepthStencil != nullptr) {
                    depthStencilView = backBufferDepthStencil->GetDepthStencilView();
                }
            }
            else if (const auto& p = renderPass.DepthStencilBuffer; p != nullptr) {
                auto nativeBuffer = std::static_pointer_cast<DepthStencilBufferDirect3D11>(p);
                POMDOG_ASSERT(nativeBuffer != nullptr);
                POMDOG_ASSERT(nativeBuffer == std::dynamic_pointer_cast<DepthStencilBufferDirect3D11>(p));
                depthStencilView = nativeBuffer->GetDepthStencilView();
            }

            if (depthStencilView != nullptr) {
                deferredContext->ClearDepthStencilView(depthStencilView, mask, depth, stencil);
            }
        }
    }
}

void GraphicsContextDirect3D11::EndRenderPass()
{
}

std::unique_ptr<Error>
GraphicsContextDirect3D11::ResizeBackBuffers(
    ID3D11Device* device, int backBufferWidthIn, int backBufferHeightIn) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(backBufferWidthIn > 0);
    POMDOG_ASSERT(backBufferHeightIn > 0);

    preferredBackBufferWidth = backBufferWidthIn;
    preferredBackBufferHeight = backBufferHeightIn;

    POMDOG_ASSERT(backBuffer != nullptr);
    backBuffer->ResetBackBuffer();

    POMDOG_ASSERT(swapChain != nullptr);
    if (auto hr = swapChain->ResizeBuffers(
            backBufferCount,
            preferredBackBufferWidth,
            preferredBackBufferHeight,
            backBufferFormat,
            0);
        FAILED(hr)) {
        return errors::New("failed to resize back buffer");
    }

    if (auto err = backBuffer->ResetBackBuffer(
            device,
            swapChain.Get(),
            preferredBackBufferWidth,
            preferredBackBufferHeight);
        err != nullptr) {
        return errors::Wrap(std::move(err), "backBuffer->ResetBackBuffer() failed");
    }

    if (backBufferDepthStencil != nullptr) {
        // TODO: MSAA
        constexpr std::int32_t multiSampleCount = 1;

        if (auto err = backBufferDepthStencil->ResetBuffer(
                device,
                preferredBackBufferWidth,
                preferredBackBufferHeight,
                backBufferDepthFormat,
                multiSampleCount);
            err != nullptr) {
            return errors::Wrap(std::move(err), "backBufferDepthStencil->ResetBuffer() failed");
        }
    }

    return nullptr;
}

ID3D11DeviceContext3* GraphicsContextDirect3D11::GetImmediateContext() noexcept
{
    POMDOG_ASSERT(immediateContext);
    return immediateContext.Get();
}

ID3D11DeviceContext3* GraphicsContextDirect3D11::GetDeferredContext() noexcept
{
    POMDOG_ASSERT(deferredContext);
    return deferredContext.Get();
}

} // namespace pomdog::gpu::detail::direct3d11
