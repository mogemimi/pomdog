// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "GraphicsContextDirect3D11.hpp"
#include "BufferDirect3D11.hpp"
#include "PipelineStateDirect3D11.hpp"
#include "RenderTarget2DDirect3D11.hpp"
#include "SamplerStateDirect3D11.hpp"
#include "Texture2DDirect3D11.hpp"
#include "../RenderSystem.DXGI/DXGIFormatHelper.hpp"
#include "../RenderSystem/GraphicsCapabilities.hpp"
#include "../RenderSystem/GraphicsCommandListImmediate.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/RenderPass.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <algorithm>

namespace Pomdog::Detail::Direct3D11 {
namespace {

using DXGI::DXGIFormatHelper;
using Microsoft::WRL::ComPtr;

D3D11_PRIMITIVE_TOPOLOGY ToD3D11PrimitiveTopology(
    PrimitiveTopology primitiveTopology) noexcept
{
    switch (primitiveTopology) {
    case PrimitiveTopology::TriangleStrip:
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    case PrimitiveTopology::TriangleList:
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case PrimitiveTopology::LineList:
        return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    case PrimitiveTopology::LineStrip:
        return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
    }
    return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
}

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
    std::shared_ptr<RenderTarget2DDirect3D11>& backBuffer)
{
    POMDOG_ASSERT(deferredContext);
    POMDOG_ASSERT(backBuffer);

    renderTargets.clear();
    renderTargets.push_back(backBuffer);

    std::array<ID3D11RenderTargetView*, 1> renderTargetViews = {
        backBuffer->GetRenderTargetView()};

    deferredContext->OMSetRenderTargets(
        static_cast<UINT>(renderTargetViews.size()),
        renderTargetViews.data(),
        renderTargets.front()->GetDepthStencilView());
}

#if defined(DEBUG) && !defined(NDEBUG)
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

} // unnamed namespace

GraphicsContextDirect3D11::GraphicsContextDirect3D11(
    HWND windowHandle,
    const Microsoft::WRL::ComPtr<IDXGIFactory1>& dxgiFactory,
    const Microsoft::WRL::ComPtr<ID3D11Device3>& device,
    const PresentationParameters& presentationParameters)
    : blendFactor({1.0f, 1.0f, 1.0f, 1.0f})
    , preferredBackBufferWidth(1)
    , preferredBackBufferHeight(1)
    , backBufferCount(2)
    , backBufferFormat(DXGIFormatHelper::ToDXGIFormat(presentationParameters.BackBufferFormat))
    , backBufferDepthFormat(presentationParameters.DepthStencilFormat)
    , needToApplyPipelineState(true)
{
    POMDOG_ASSERT(device);

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

#if defined(DEBUG) && !defined(NDEBUG)
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
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to create deferred context");
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
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create SwapChain");
        }
    }
    {
        ///@todo MSAA is not implemented yet
        constexpr int multiSampleCount = 1;

        backBuffer = std::make_shared<RenderTarget2DDirect3D11>(
            device.Get(),
            swapChain.Get(),
            preferredBackBufferWidth,
            preferredBackBufferHeight,
            backBufferDepthFormat,
            multiSampleCount);

        renderTargets.reserve(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
        renderTargets.push_back(backBuffer);
    }

    textureResourceViews.fill(nullptr);

    // NOTE: Set default values for graphics context
    this->SetBlendFactor(Vector4{1.0f, 1.0f, 1.0f, 1.0f});

#if defined(DEBUG) && !defined(NDEBUG)
    auto graphicsCapbilities = this->GetCapabilities();

    POMDOG_ASSERT(graphicsCapbilities.SamplerSlotCount > 0);
    weakTextures.resize(graphicsCapbilities.SamplerSlotCount);
#endif
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
    const std::vector<std::shared_ptr<GraphicsCommandListImmediate>>& commandLists)
{
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

    if (needToApplyPipelineState) {
        pipelineState->Apply(deferredContext.Get(), blendFactor.data());
        needToApplyPipelineState = false;
    }
}

void GraphicsContextDirect3D11::Draw(
    std::size_t vertexCount,
    std::size_t startVertexLocation)
{
    POMDOG_ASSERT(deferredContext);
#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets, weakTextures);
#endif

    ApplyPipelineState();

    deferredContext->Draw(static_cast<UINT>(vertexCount), static_cast<UINT>(startVertexLocation));
}

void GraphicsContextDirect3D11::DrawIndexed(
    std::size_t indexCount,
    std::size_t startIndexLocation)
{
    POMDOG_ASSERT(deferredContext);
#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets, weakTextures);
#endif

    ApplyPipelineState();

    deferredContext->DrawIndexed(static_cast<UINT>(indexCount), static_cast<UINT>(startIndexLocation), 0);
}

void GraphicsContextDirect3D11::DrawInstanced(
    std::size_t vertexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startVertexLocation,
    std::size_t startInstanceLocation)
{
    POMDOG_ASSERT(deferredContext);
#if defined(DEBUG) && !defined(NDEBUG)
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
    std::size_t indexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startIndexLocation,
    std::size_t startInstanceLocation)
{
    POMDOG_ASSERT(deferredContext);
#if defined(DEBUG) && !defined(NDEBUG)
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
    POMDOG_ASSERT(deferredContext);
    POMDOG_ASSERT(indexBuffer);

    auto nativeIndexBuffer = static_cast<BufferDirect3D11*>(
        indexBuffer->GetNativeIndexBuffer());

    POMDOG_ASSERT(nativeIndexBuffer != nullptr);
    POMDOG_ASSERT(nativeIndexBuffer == dynamic_cast<BufferDirect3D11*>(
        indexBuffer->GetNativeIndexBuffer()));

    deferredContext->IASetIndexBuffer(nativeIndexBuffer->GetBuffer(),
        DXGIFormatHelper::ToDXGIFormat(indexBuffer->GetElementSize()), 0);
}

GraphicsCapabilities GraphicsContextDirect3D11::GetCapabilities() const
{
    GraphicsCapabilities caps;
    caps.SamplerSlotCount = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT;
    caps.ConstantBufferSlotCount = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
    return std::move(caps);
}

void GraphicsContextDirect3D11::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
{
    POMDOG_ASSERT(deferredContext);
    deferredContext->IASetPrimitiveTopology(
        ToD3D11PrimitiveTopology(primitiveTopology));
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
    int index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer,
    std::size_t offset)
{
    POMDOG_ASSERT(vertexBuffer != nullptr);
    POMDOG_ASSERT(vertexBuffer->GetNativeVertexBuffer() != nullptr);

    auto nativeVertexBuffer = static_cast<BufferDirect3D11*>(
        vertexBuffer->GetNativeVertexBuffer());

    POMDOG_ASSERT(nativeVertexBuffer != nullptr);
    POMDOG_ASSERT(nativeVertexBuffer == dynamic_cast<BufferDirect3D11*>(
        vertexBuffer->GetNativeVertexBuffer()));

    const auto buffer = nativeVertexBuffer->GetBuffer();
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

void GraphicsContextDirect3D11::SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineStateIn)
{
    POMDOG_ASSERT(pipelineStateIn);

    if (pipelineState != pipelineStateIn) {
        pipelineState = std::dynamic_pointer_cast<PipelineStateDirect3D11>(pipelineStateIn);
        POMDOG_ASSERT(pipelineState);

        needToApplyPipelineState = true;
    }
}

void GraphicsContextDirect3D11::SetConstantBuffer(
    int index,
    const std::shared_ptr<NativeBuffer>& constantBufferIn,
    std::size_t offset,
    std::size_t sizeInBytes)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
    POMDOG_ASSERT(dynamic_cast<BufferDirect3D11*>(constantBufferIn.get()));

    auto& constantBuffer = static_cast<BufferDirect3D11&>(*constantBufferIn);
    auto buffer = constantBuffer.GetBuffer();
    POMDOG_ASSERT(buffer != nullptr);

    const auto startOffset = static_cast<UINT>(offset);
    const auto constantSize = static_cast<UINT>(sizeInBytes);

    POMDOG_ASSERT(deferredContext);
    deferredContext->VSSetConstantBuffers1(index, 1, &buffer, &startOffset, &constantSize);
    deferredContext->PSSetConstantBuffers1(index, 1, &buffer, &startOffset, &constantSize);
}

void GraphicsContextDirect3D11::SetSampler(int index, const std::shared_ptr<NativeSamplerState>& samplerIn)
{
    POMDOG_ASSERT(index >= 0);
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

void GraphicsContextDirect3D11::SetTexture(int index)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
    POMDOG_ASSERT(index < static_cast<int>(textureResourceViews.size()));

#if defined(DEBUG) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures.empty());
    POMDOG_ASSERT(index < static_cast<int>(weakTextures.size()));
    weakTextures[index].reset();
#endif

    textureResourceViews[index] = nullptr;

    POMDOG_ASSERT(deferredContext);
    deferredContext->PSSetShaderResources(index, 1, &textureResourceViews[index]);
}

void GraphicsContextDirect3D11::SetTexture(int index, const std::shared_ptr<Texture2D>& textureIn)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
    POMDOG_ASSERT(index < static_cast<int>(textureResourceViews.size()));
    POMDOG_ASSERT(textureIn);
    POMDOG_ASSERT(textureIn->GetNativeTexture2D() != nullptr);

#if defined(DEBUG) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures.empty());
    POMDOG_ASSERT(index < static_cast<int>(weakTextures.size()));
    weakTextures[index] = textureIn;
#endif

    auto texture = static_cast<Texture2DDirect3D11*>(textureIn->GetNativeTexture2D());

    POMDOG_ASSERT(texture != nullptr);
    POMDOG_ASSERT(texture == dynamic_cast<Texture2DDirect3D11*>(textureIn->GetNativeTexture2D()));

    textureResourceViews[index] = texture->GetShaderResourceView();

    POMDOG_ASSERT(deferredContext);
    deferredContext->PSSetShaderResources(0, 1, &textureResourceViews[index]);
}

void GraphicsContextDirect3D11::SetTexture(int index, const std::shared_ptr<RenderTarget2D>& textureIn)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
    POMDOG_ASSERT(index < static_cast<int>(textureResourceViews.size()));
    POMDOG_ASSERT(textureIn);
    POMDOG_ASSERT(textureIn->GetNativeRenderTarget2D() != nullptr);

#if defined(DEBUG) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures.empty());
    POMDOG_ASSERT(index < static_cast<int>(weakTextures.size()));
    weakTextures[index] = textureIn;
#endif

    auto texture = static_cast<RenderTarget2DDirect3D11*>(textureIn->GetNativeRenderTarget2D());

    POMDOG_ASSERT(texture != nullptr);
    POMDOG_ASSERT(texture == dynamic_cast<RenderTarget2DDirect3D11*>(textureIn->GetNativeRenderTarget2D()));

    textureResourceViews[index] = texture->GetShaderResourceView();

    POMDOG_ASSERT(deferredContext);
    deferredContext->PSSetShaderResources(0, 1, &textureResourceViews[index]);
}

void GraphicsContextDirect3D11::SetRenderPass(const RenderPass& renderPass)
{
    POMDOG_ASSERT(deferredContext);
    POMDOG_ASSERT(!renderPass.RenderTargets.empty());
    POMDOG_ASSERT(renderPass.RenderTargets.size() == 8);

    const bool useBackBuffer = (std::get<0>(renderPass.RenderTargets.front()) == nullptr);

    if (useBackBuffer) {
        UseBackBufferAsRenderTarget(deferredContext, renderTargets, backBuffer);
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
            auto nativeRenderTarget = renderTarget->GetNativeRenderTarget2D();
            auto direct3d11RenderTarget = static_cast<RenderTarget2DDirect3D11*>(nativeRenderTarget);
            POMDOG_ASSERT(direct3d11RenderTarget != nullptr);
            POMDOG_ASSERT(direct3d11RenderTarget == dynamic_cast<RenderTarget2DDirect3D11*>(nativeRenderTarget));

            renderTargets.emplace_back(renderTarget, direct3d11RenderTarget);
            renderTargetViews[i] = direct3d11RenderTarget->GetRenderTargetView();
            POMDOG_ASSERT(renderTargetViews[i] != nullptr);
            POMDOG_ASSERT(i <= renderTargets.size());
        }

        deferredContext->OMSetRenderTargets(
            static_cast<UINT>(renderTargets.size()),
            renderTargetViews.data(),
            renderTargets.front()->GetDepthStencilView());
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
            POMDOG_ASSERT(!renderTargets.empty());
            POMDOG_ASSERT(renderTargets.size() <= renderPass.RenderTargets.size());
            POMDOG_ASSERT(renderTargets.front());

            auto depthStencilView = renderTargets.front()->GetDepthStencilView();
            if (depthStencilView != nullptr) {
                deferredContext->ClearDepthStencilView(
                    depthStencilView, mask, depth, stencil);
            }
        }
    }
}

void GraphicsContextDirect3D11::ResizeBackBuffers(
    ID3D11Device* device, int backBufferWidthIn, int backBufferHeightIn)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(backBufferWidthIn > 0);
    POMDOG_ASSERT(backBufferHeightIn > 0);

    preferredBackBufferWidth = backBufferWidthIn;
    preferredBackBufferHeight = backBufferHeightIn;

    POMDOG_ASSERT(backBuffer);
    backBuffer->ResetBackBuffer();

    POMDOG_ASSERT(swapChain);
    auto hr = swapChain->ResizeBuffers(
        backBufferCount,
        preferredBackBufferWidth,
        preferredBackBufferHeight,
        backBufferFormat,
        0);

    if (FAILED(hr)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to resize back buffer");
    }

    backBuffer->ResetBackBuffer(
        device,
        swapChain.Get(),
        preferredBackBufferWidth,
        preferredBackBufferHeight,
        backBufferDepthFormat);
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

} // namespace Pomdog::Detail::Direct3D11
