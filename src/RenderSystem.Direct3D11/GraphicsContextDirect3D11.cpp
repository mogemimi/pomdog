// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "GraphicsContextDirect3D11.hpp"
#include "BufferDirect3D11.hpp"
#include "ConstantLayoutDirect3D11.hpp"
#include "PipelineStateDirect3D11.hpp"
#include "RenderTarget2DDirect3D11.hpp"
#include "SamplerStateDirect3D11.hpp"
#include "Texture2DDirect3D11.hpp"
#include "../RenderSystem.DXGI/DXGIFormatHelper.hpp"
#include "../RenderSystem/GraphicsCapabilities.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Graphics/ClearOptions.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/StringFormat.hpp"
#include <algorithm>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

using DXGI::DXGIFormatHelper;

static D3D11_PRIMITIVE_TOPOLOGY ToD3D11PrimitiveTopology(PrimitiveTopology primitiveTopology) noexcept
{
    switch (primitiveTopology) {
    case PrimitiveTopology::TriangleStrip: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    case PrimitiveTopology::TriangleList: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case PrimitiveTopology::LineList: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    case PrimitiveTopology::LineStrip: return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
    }
    return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
}
//-----------------------------------------------------------------------
static void ChooseMultiSampleSetting(
    ID3D11Device* device,
    DXGI_FORMAT backBufferFormat,
    int preferredMultiSampleCount,
    DXGI_SAMPLE_DESC & sampleDesc)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(preferredMultiSampleCount >= 1);
    POMDOG_ASSERT(preferredMultiSampleCount <= 32);

    const auto maxSampleCount = std::min(
        preferredMultiSampleCount,
        D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT);

    for (int sampleCount = maxSampleCount; sampleCount >= 1; --sampleCount)
    {
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

} // unnamed namespace
//-----------------------------------------------------------------------
GraphicsContextDirect3D11::GraphicsContextDirect3D11(
    HWND windowHandle,
    Microsoft::WRL::ComPtr<IDXGIFactory1> const& dxgiFactory,
    Microsoft::WRL::ComPtr<ID3D11Device> const& device,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContextIn,
    PresentationParameters const& presentationParameters)
    : deviceContext(deviceContextIn)
    , blendFactor({1.0f, 1.0f, 1.0f, 1.0f})
    , preferredBackBufferWidth(1)
    , preferredBackBufferHeight(1)
    , backBufferCount(2)
    , backBufferFormat(DXGIFormatHelper::ToDXGIFormat(presentationParameters.BackBufferFormat))
    , backBufferDepthFormat(presentationParameters.DepthStencilFormat)
    , needToApplyPipelineState(true)
{
    using Microsoft::WRL::ComPtr;

    POMDOG_ASSERT(device);
    POMDOG_ASSERT(deviceContext);

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
    Log::Internal(StringFormat(
        "DXGI_SAMPLE_DESC.Count  : %d\n"
        "DXGI_SAMPLE_DESC.Quality: %d",
        sampleDesc.Count,
        sampleDesc.Quality));
#endif

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
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = windowHandle;
        swapChainDesc.Windowed = TRUE;
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

    SetRenderTarget();

    textureResourceViews.fill(nullptr);
}
//-----------------------------------------------------------------------
GraphicsContextDirect3D11::~GraphicsContextDirect3D11()
{
    textureResourceViews.fill(nullptr);
    renderTargets.clear();
    backBuffer.reset();
    swapChain.Reset();
    deviceContext.Reset();
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::Clear(Color const& color)
{
    auto const fillColor = color.ToVector4();

    for (auto & renderTarget: renderTargets)
    {
        POMDOG_ASSERT(renderTarget);
        deviceContext->ClearRenderTargetView(
            renderTarget->GetRenderTargetView(), fillColor.Data());

        auto depthStencilView = renderTarget->GetDepthStencilView();
        if (depthStencilView != nullptr) {
            constexpr UINT mask = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
            deviceContext->ClearDepthStencilView(
                depthStencilView, mask, 1.0f, 0);
        }
    }
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::Clear(ClearOptions options, Color const& color, float depth, std::uint8_t stencil)
{
    POMDOG_ASSERT(stencil <= std::numeric_limits<UINT8>::max());
    POMDOG_ASSERT(stencil >= 0);

    auto const fillColor = color.ToVector4();

    UINT mask = 0;

    if ((options | ClearOptions::DepthBuffer) == options) {
        mask |= D3D11_CLEAR_DEPTH;
    }
    if ((options | ClearOptions::Stencil) == options) {
        mask |= D3D11_CLEAR_STENCIL;
    }

    for (auto & renderTarget : renderTargets)
    {
        POMDOG_ASSERT(renderTarget);

        if ((options | ClearOptions::RenderTarget) == options) {
            deviceContext->ClearRenderTargetView(
                renderTarget->GetRenderTargetView(), fillColor.Data());
        }

        auto depthStencilView = renderTarget->GetDepthStencilView();
        if (depthStencilView != nullptr) {
            deviceContext->ClearDepthStencilView(
                depthStencilView, mask, depth, stencil);
        }
    }
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::Present()
{
    POMDOG_ASSERT(swapChain);
    swapChain->Present(0, 0);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::ApplyPipelineState()
{
    POMDOG_ASSERT(pipelineState);

    if (needToApplyPipelineState) {
        pipelineState->Apply(deviceContext.Get(), blendFactor.data());
        needToApplyPipelineState = false;
    }
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::Draw(std::uint32_t vertexCount)
{
    POMDOG_ASSERT(deviceContext);

    ApplyPipelineState();

    deviceContext->Draw(vertexCount, 0);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::DrawIndexed(std::uint32_t indexCount)
{
    POMDOG_ASSERT(deviceContext);

    ApplyPipelineState();

    deviceContext->DrawIndexed(indexCount, 0, 0);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::DrawInstanced(std::uint32_t vertexCount, std::uint32_t instanceCount)
{
    POMDOG_ASSERT(deviceContext);

    ApplyPipelineState();

    deviceContext->DrawInstanced(vertexCount, instanceCount, 0, 0);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::DrawIndexedInstanced(
    std::uint32_t indexCount, std::uint32_t instanceCount)
{
    POMDOG_ASSERT(deviceContext);

    ApplyPipelineState();

    deviceContext->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetIndexBuffer(std::shared_ptr<IndexBuffer> const& indexBuffer)
{
    POMDOG_ASSERT(indexBuffer);

    auto nativeIndexBuffer = static_cast<BufferDirect3D11*>(
        indexBuffer->NativeIndexBuffer());

    POMDOG_ASSERT(nativeIndexBuffer != nullptr);
    POMDOG_ASSERT(nativeIndexBuffer == dynamic_cast<BufferDirect3D11*>(
        indexBuffer->NativeIndexBuffer()));

    deviceContext->IASetIndexBuffer(nativeIndexBuffer->GetBuffer(),
        DXGIFormatHelper::ToDXGIFormat(indexBuffer->ElementSize()), 0);
}
//-----------------------------------------------------------------------
GraphicsCapabilities GraphicsContextDirect3D11::GetCapabilities() const
{
    GraphicsCapabilities caps;
    caps.SamplerSlotCount = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT;
    return std::move(caps);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetViewport(Viewport const& viewportIn)
{
    POMDOG_ASSERT(0.0f <= viewportIn.Width());
    POMDOG_ASSERT(0.0f <= viewportIn.Height());
    POMDOG_ASSERT(D3D11_VIEWPORT_BOUNDS_MAX >= viewportIn.TopLeftX() + viewportIn.Width());
    POMDOG_ASSERT(D3D11_VIEWPORT_BOUNDS_MAX >= viewportIn.TopLeftY() + viewportIn.Height());

    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<FLOAT>(viewportIn.Width());
    viewport.Height = static_cast<FLOAT>(viewportIn.Height());
    viewport.MinDepth = viewportIn.MinDepth;
    viewport.MaxDepth = viewportIn.MaxDepth;
    viewport.TopLeftX = static_cast<FLOAT>(viewportIn.TopLeftX());
    viewport.TopLeftY = static_cast<FLOAT>(viewportIn.TopLeftY());

    POMDOG_ASSERT(deviceContext);
    deviceContext->RSSetViewports(1, &viewport);
}
//-----------------------------------------------------------------------
Rectangle GraphicsContextDirect3D11::GetScissorRectangle() const
{
    ///@todo Not implemented
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetScissorRectangle(Rectangle const& rectangle)
{
    POMDOG_ASSERT(deviceContext);

    std::vector<D3D11_RECT> rects;
    rects.resize(std::max<std::size_t>(1, renderTargets.size()));

    D3D11_RECT rect;
    rect.bottom = rectangle.Bottom();
    rect.left = rectangle.Left();
    rect.right = rectangle.Right();
    rect.top = rectangle.Top();

    std::fill(std::begin(rects), std::end(rects), rect);

    POMDOG_ASSERT(!rects.empty());

    deviceContext->RSSetScissorRects(rects.size(), rects.data());
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
{
    deviceContext->IASetPrimitiveTopology(
        ToD3D11PrimitiveTopology(primitiveTopology));
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetBlendFactor(Color const& blendFactorIn)
{
    auto vec = blendFactorIn.ToVector4();
    blendFactor[0] = vec.X;
    blendFactor[1] = vec.Y;
    blendFactor[2] = vec.Z;
    blendFactor[3] = vec.W;
    needToApplyPipelineState = true;
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffersIn)
{
    POMDOG_ASSERT(!vertexBuffersIn.empty());

    std::vector<UINT> strides;
    strides.reserve(vertexBuffersIn.size());

    std::vector<UINT> offsets;
    offsets.reserve(vertexBuffersIn.size());

    std::vector<ID3D11Buffer*> vertexBuffers;
    vertexBuffers.reserve(vertexBuffersIn.size());

    for (auto & vertexBuffer: vertexBuffersIn)
    {
        POMDOG_ASSERT(vertexBuffer);
        POMDOG_ASSERT(vertexBuffer->NativeVertexBuffer());

        auto nativeVertexBuffer = static_cast<BufferDirect3D11*>(
            vertexBuffer->NativeVertexBuffer());

        POMDOG_ASSERT(nativeVertexBuffer != nullptr);
        POMDOG_ASSERT(nativeVertexBuffer == dynamic_cast<BufferDirect3D11*>(
            vertexBuffer->NativeVertexBuffer()));

        vertexBuffers.push_back(nativeVertexBuffer->GetBuffer());
        strides.push_back(vertexBuffer->StrideBytes());
        offsets.push_back(0);
    }

    POMDOG_ASSERT(deviceContext);
    deviceContext->IASetVertexBuffers(0, vertexBuffers.size(), vertexBuffers.data(),
        strides.data(), offsets.data());
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetSampler(int index, NativeSamplerState* samplerIn)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
    POMDOG_ASSERT(samplerIn != nullptr);

    auto sampler = static_cast<SamplerStateDirect3D11*>(samplerIn);

    POMDOG_ASSERT(sampler != nullptr);
    POMDOG_ASSERT(sampler == dynamic_cast<SamplerStateDirect3D11*>(samplerIn));
    POMDOG_ASSERT(sampler->GetSamplerState() != nullptr);

    std::array<ID3D11SamplerState*, 1> const states = {
        sampler->GetSamplerState() };

    POMDOG_ASSERT(deviceContext);
    deviceContext->PSSetSamplers(index, states.size(), states.data());
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetTexture(int index)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
    POMDOG_ASSERT(index < static_cast<int>(textureResourceViews.size()));

    textureResourceViews[index] = nullptr;

    POMDOG_ASSERT(deviceContext);
    deviceContext->PSSetShaderResources(index, 1, &textureResourceViews[index]);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetTexture(int index, Texture2D & textureIn)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
    POMDOG_ASSERT(index < static_cast<int>(textureResourceViews.size()));
    POMDOG_ASSERT(textureIn.NativeTexture2D() != nullptr);

    auto texture = static_cast<Texture2DDirect3D11*>(textureIn.NativeTexture2D());

    POMDOG_ASSERT(texture != nullptr);
    POMDOG_ASSERT(texture == dynamic_cast<Texture2DDirect3D11*>(textureIn.NativeTexture2D()));

    textureResourceViews[index] = texture->GetShaderResourceView();

    POMDOG_ASSERT(deviceContext);
    deviceContext->PSSetShaderResources(0, 1, &textureResourceViews[index]);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetTexture(int index, RenderTarget2D & textureIn)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
    POMDOG_ASSERT(index < static_cast<int>(textureResourceViews.size()));
    POMDOG_ASSERT(textureIn.NativeRenderTarget2D() != nullptr);

    auto texture = static_cast<RenderTarget2DDirect3D11*>(textureIn.NativeRenderTarget2D());

    POMDOG_ASSERT(texture != nullptr);
    POMDOG_ASSERT(texture == dynamic_cast<RenderTarget2DDirect3D11*>(textureIn.NativeRenderTarget2D()));

    textureResourceViews[index] = texture->GetShaderResourceView();

    POMDOG_ASSERT(deviceContext);
    deviceContext->PSSetShaderResources(0, 1, &textureResourceViews[index]);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetRenderTarget()
{
    renderTargets.clear();
    renderTargets.push_back(backBuffer);

    std::array<ID3D11RenderTargetView*, 1> renderTargetViews = {
        backBuffer->GetRenderTargetView() };

    deviceContext->OMSetRenderTargets(
        renderTargetViews.size(),
        renderTargetViews.data(),
        renderTargets.front()->GetDepthStencilView());
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargetsIn)
{
    POMDOG_ASSERT(!renderTargetsIn.empty());
    POMDOG_ASSERT(renderTargetsIn.size() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
    POMDOG_ASSERT(renderTargetsIn.size() <= renderTargets.capacity());

    renderTargets.clear();

    std::array<ID3D11RenderTargetView*,
        D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> renderTargetViews;

    POMDOG_ASSERT(renderTargetViews.size() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
    POMDOG_ASSERT(renderTargetViews.size() >= renderTargetsIn.size());

    for (std::size_t i = 0; i < renderTargetsIn.size(); ++i)
    {
        POMDOG_ASSERT(renderTargetsIn[i]);

        auto renderTarget = static_cast<RenderTarget2DDirect3D11*>(renderTargetsIn[i]->NativeRenderTarget2D());
        POMDOG_ASSERT(renderTarget != nullptr);
        POMDOG_ASSERT(renderTarget == dynamic_cast<RenderTarget2DDirect3D11*>(renderTargetsIn[i]->NativeRenderTarget2D()));

        renderTargets.emplace_back(renderTargetsIn[i], renderTarget);

        renderTargetViews[i] = renderTarget->GetRenderTargetView();
        POMDOG_ASSERT(renderTargetViews[i] != nullptr);
    }

    deviceContext->OMSetRenderTargets(
        renderTargetsIn.size(),
        renderTargetViews.data(),
        renderTargets.front()->GetDepthStencilView());
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetPipelineState(std::shared_ptr<NativePipelineState> const& pipelineStateIn)
{
    POMDOG_ASSERT(deviceContext);
    POMDOG_ASSERT(pipelineStateIn);

    if (pipelineState != pipelineStateIn) {
        pipelineState = std::dynamic_pointer_cast<PipelineStateDirect3D11>(pipelineStateIn);
        POMDOG_ASSERT(pipelineState);

        needToApplyPipelineState = true;
    }
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetConstantBuffers(std::shared_ptr<NativeConstantLayout> const& constantLayoutIn)
{
    POMDOG_ASSERT(deviceContext);

    POMDOG_ASSERT(constantLayoutIn);
    POMDOG_ASSERT(dynamic_cast<ConstantLayoutDirect3D11*>(constantLayoutIn.get()));

    auto & constantLayout = static_cast<ConstantLayoutDirect3D11&>(*constantLayoutIn);
    constantLayout.Apply(deviceContext.Get());
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::ResizeBackBuffers(ID3D11Device* device,
    int backBufferWidthIn, int backBufferHeightIn)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(backBufferWidthIn > 0);
    POMDOG_ASSERT(backBufferHeightIn > 0);

    preferredBackBufferWidth = backBufferWidthIn;
    preferredBackBufferHeight = backBufferHeightIn;

    bool isBackBufferActive = false;

    if (!renderTargets.empty()) {
        isBackBufferActive = (backBuffer == renderTargets.front());
    }

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

    if (isBackBufferActive) {
        SetRenderTarget();
    }
}
//-----------------------------------------------------------------------
ID3D11DeviceContext* GraphicsContextDirect3D11::GetDeviceContext()
{
    POMDOG_ASSERT(deviceContext);
    return deviceContext.Get();
}
//-----------------------------------------------------------------------
} // namespace Direct3D11
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog
