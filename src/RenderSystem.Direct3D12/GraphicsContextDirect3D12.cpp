// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "GraphicsContextDirect3D12.hpp"
#include "BufferDirect3D12.hpp"
#include "ConstantLayoutDirect3D12.hpp"
#include "PipelineStateDirect3D12.hpp"
#include "RenderTarget2DDirect3D12.hpp"
#include "SamplerStateDirect3D12.hpp"
#include "Texture2DDirect3D12.hpp"
#include "../RenderSystem.DXGI/DXGIFormatHelper.hpp"
#include "../RenderSystem/GraphicsCapabilities.hpp"
#include "Pomdog/Graphics/ClearOptions.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/StringFormat.hpp"
#include <algorithm>
#include <array>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {
namespace {

using Microsoft::WRL::ComPtr;
using DXGI::DXGIFormatHelper;

static D3D_PRIMITIVE_TOPOLOGY ToPrimitiveTopology(PrimitiveTopology primitiveTopology) noexcept
{
    switch (primitiveTopology) {
    case PrimitiveTopology::TriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    case PrimitiveTopology::TriangleList: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case PrimitiveTopology::LineList: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    case PrimitiveTopology::LineStrip: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
    }
    return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
}
//-----------------------------------------------------------------------
static D3D12_RESOURCE_BARRIER CreateResourceBarrier(
    ID3D12Resource* resource,
    D3D12_RESOURCE_STATES stateBefore,
    D3D12_RESOURCE_STATES stateAfter) noexcept
{
    POMDOG_ASSERT(resource);
    D3D12_RESOURCE_BARRIER barrrier;
    barrrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrrier.Transition.pResource = resource;
    barrrier.Transition.StateBefore = stateBefore;
    barrrier.Transition.StateAfter = stateAfter;
    barrrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    return std::move(barrrier);
}

} // unnamed namespace
//-----------------------------------------------------------------------
GraphicsContextDirect3D12::GraphicsContextDirect3D12(
    Microsoft::WRL::ComPtr<ID3D12Device> const& device,
    HWND windowHandle,
    PresentationParameters const& presentationParameters)
    : preferredBackBufferWidth(presentationParameters.BackBufferWidth)
    , preferredBackBufferHeight(presentationParameters.BackBufferHeight)
    , backBufferFormat(DXGIFormatHelper::ToDXGIFormat(presentationParameters.BackBufferFormat))
    , backBufferDepthFormat(presentationParameters.DepthStencilFormat)
    , needToApplyRootParameters(true)
{
    POMDOG_ASSERT(device);
    POMDOG_ASSERT(preferredBackBufferWidth > 0);
    POMDOG_ASSERT(preferredBackBufferHeight > 0);

    // Create command allocator
    HRESULT hr = device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&commandAllocator));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create command allocator");
    }

    // Create command queue
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
    ZeroMemory(&commandQueueDesc, sizeof(commandQueueDesc));
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create command queue");
    }

    // Create DXGI factory
    ComPtr<IDXGIFactory3> dxgiFactory;
    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create DXGI factory");
    }

    ///@todo Multisampling is not supported yet
    DXGI_SAMPLE_DESC sampleDesc;
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

#if defined(DEBUG) && !defined(NDEBUG)
    Log::Internal(StringFormat(
        "DXGI_SAMPLE_DESC.Count  : %d\n"
        "DXGI_SAMPLE_DESC.Quality: %d",
        sampleDesc.Count,
        sampleDesc.Quality));
#endif

    // Create swap chain
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
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapChainDesc.OutputWindow = windowHandle;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Windowed = (!presentationParameters.IsFullScreen ? TRUE : FALSE);
    swapChainDesc.SampleDesc.Count = sampleDesc.Count;
    swapChainDesc.SampleDesc.Quality = sampleDesc.Quality;

    hr = dxgiFactory->CreateSwapChain(commandQueue.Get(), &swapChainDesc,
        reinterpret_cast<IDXGISwapChain**>(swapChain.GetAddressOf()));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create swap chain");
    }

    // Create graphics command list
    POMDOG_ASSERT(commandAllocator);

    hr = device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        commandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(&commandList));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create graphics command list");
    }

    hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create fence");
    }

    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    {
        ///@todo MSAA is not implemented yet
        constexpr int multiSampleCount = 1;

        backBuffer = std::make_shared<RenderTarget2DDirect3D12>(
            device.Get(),
            swapChain.Get(),
            preferredBackBufferWidth,
            preferredBackBufferHeight,
            backBufferFormat,
            backBufferDepthFormat,
            multiSampleCount);

        renderTargets.reserve(D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
        renderTargets.push_back(backBuffer);
    }

    SetRenderTarget();

    textures.fill(nullptr);
    samplers.fill(nullptr);
}
//-----------------------------------------------------------------------
GraphicsContextDirect3D12::~GraphicsContextDirect3D12()
{
    textures.fill(nullptr);
    samplers.fill(nullptr);

    CloseHandle(fenceEvent);
    fence.Reset();
    commandList.Reset();
    swapChain.Reset();
    commandQueue.Reset();
    commandAllocator.Reset();
}
//-----------------------------------------------------------------------
GraphicsCapabilities GraphicsContextDirect3D12::GetCapabilities() const
{
    GraphicsCapabilities caps;
    caps.SamplerSlotCount = D3D12_COMMONSHADER_SAMPLER_SLOT_COUNT;
    return std::move(caps);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::BackBufferBarrier()
{
    POMDOG_ASSERT(backBuffer);
    auto barrierDesc = CreateResourceBarrier(
        backBuffer->GetTexture2D(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);

    POMDOG_ASSERT(commandList);
    commandList->ResourceBarrier(1, &barrierDesc);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::Clear(ClearOptions options,
    Color const& color, float depth, std::uint8_t stencil)
{
    POMDOG_ASSERT(stencil <= std::numeric_limits<UINT8>::max());
    POMDOG_ASSERT(stencil >= 0);

    POMDOG_ASSERT(commandList);

    auto const fillColor = color.ToVector4();

    D3D12_CLEAR_FLAGS clearFlags = static_cast<D3D12_CLEAR_FLAGS>(0);

    if ((options | ClearOptions::DepthBuffer) == options) {
        clearFlags |= D3D12_CLEAR_FLAG_DEPTH;
    }
    if ((options | ClearOptions::Stencil) == options) {
        clearFlags |= D3D12_CLEAR_FLAG_STENCIL;
    }

    bool const clearRTVEnabled = (options | ClearOptions::RenderTarget) == options;
    bool const clearDSVEnabled = (clearFlags != 0);

    for (auto & renderTarget : renderTargets)
    {
        POMDOG_ASSERT(renderTarget);

        constexpr D3D12_RECT* rects = nullptr;
        constexpr UINT rectCount = 0;

        if (clearRTVEnabled) {
            auto renderTargetView = renderTarget->GetRenderTargetDescriptorHeap();
            commandList->ClearRenderTargetView(
                renderTargetView->GetCPUDescriptorHandleForHeapStart(),
                fillColor.Data(), rectCount, rects);
        }

        auto depthStencilView = renderTarget->GetDepthStencilDescriptorHeap();
        if (clearDSVEnabled && depthStencilView != nullptr) {
            commandList->ClearDepthStencilView(
                depthStencilView->GetCPUDescriptorHandleForHeapStart(),
                clearFlags, depth, stencil,
                rectCount, rects);
        }
    }
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::Present()
{
    POMDOG_ASSERT(backBuffer);
    POMDOG_ASSERT(commandAllocator);
    POMDOG_ASSERT(commandQueue);
    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(swapChain);
    POMDOG_ASSERT(fence);

    ///@note Chuck Norris can break resource barrier in the DirectX 12.
    auto barrierDesc = CreateResourceBarrier(
        backBuffer->GetTexture2D(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);

    commandList->ResourceBarrier(1, &barrierDesc);

    HRESULT hr = commandList->Close();
    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to close command list");
    }

    std::array<ID3D12CommandList*, 1> const commandLists = { commandList.Get() };
    commandQueue->ExecuteCommandLists(commandLists.size(), commandLists.data());

    hr = swapChain->Present(1, 0);
    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to present swap chain");
    }

    ///@todo FIXME
    constexpr UINT64 importantNumber = 42;
    fence->Signal(0);
    fence->SetEventOnCompletion(importantNumber, fenceEvent);
    hr = commandQueue->Signal(fence.Get(), importantNumber);
    auto waitResult = WaitForSingleObject(fenceEvent, INFINITE);
    if (waitResult != WAIT_OBJECT_0) {
        if (waitResult != WAIT_FAILED) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to WaitForSingleObject");
        }
        ///@todo WAIT_TIMEOUT and WAIT_ABANDONED not implemented yet
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Not implemented");
    }

    hr = commandAllocator->Reset();
    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to reset command allocator");
    }

    hr = commandList->Reset(commandAllocator.Get(), nullptr);
    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to reset command list");
    }
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::BeginDraw()
{
    POMDOG_ASSERT(pipelineState);

    if (!needToApplyRootParameters) {
        return;
    }

    const auto constantBufferCount = pipelineState->GetConstantBufferCount();
    const auto textureCount = pipelineState->GetSahderResourceCount();
    const auto samplerCount = pipelineState->GetSamplerCount();

    // Set descriptor heaps
    std::vector<ID3D12DescriptorHeap*> descriptorHeaps;

    for (std::size_t i = 0; i < textureCount; ++i) {
        POMDOG_ASSERT(i < textures.size());
        auto & texture = textures[i];
        descriptorHeaps.push_back(texture);
    }

    for (std::size_t i = 0; i < samplerCount; ++i) {
        POMDOG_ASSERT(i < samplers.size());
        auto & sampler = samplers[i];
        descriptorHeaps.push_back(sampler->GetDescriptorHeap());
    }

    if (!descriptorHeaps.empty()) {
        commandList->SetDescriptorHeaps(descriptorHeaps.size(), descriptorHeaps.data());
    }
    else {
        commandList->SetDescriptorHeaps(0, nullptr);
    }

    // Set root parameters
    UINT rootParameterIndex = 0;

    auto & constantBufferViews = constantLayout->GetConstantBufferViews();

    POMDOG_ASSERT(constantBufferCount <= constantBufferViews.size());
    POMDOG_ASSERT(textureCount <= textures.size());
    POMDOG_ASSERT(samplerCount <= samplers.size());

    if (constantBufferCount > constantBufferViews.size()) {
        ///@todo throw exception
        return;
    }
    if (textureCount > textures.size()) {
        ///@todo throw exception
        return;
    }
    if (samplerCount > samplers.size()) {
        ///@todo throw exception
        return;
    }

    for (std::size_t i = 0; i < constantBufferCount; ++i)
    {
        POMDOG_ASSERT(i < constantBufferViews.size());
        auto & constantBuffer = constantBufferViews[i].ConstantBuffer;

        commandList->SetGraphicsRootConstantBufferView(
            rootParameterIndex,
            constantBuffer->GetGPUVirtualAddress());

        ++rootParameterIndex;
    }

    for (std::size_t i = 0; i < textureCount; ++i)
    {
        POMDOG_ASSERT(i < textures.size());
        POMDOG_ASSERT(textures[i]);
        auto descriptorHeap = textures[i];

        POMDOG_ASSERT(descriptorHeap != nullptr);

        commandList->SetGraphicsRootDescriptorTable(
            rootParameterIndex,
            descriptorHeap->GetGPUDescriptorHandleForHeapStart());

        ++rootParameterIndex;
    }

    for (std::size_t i = 0; i < samplerCount; ++i)
    {
        POMDOG_ASSERT(i < samplers.size());
        POMDOG_ASSERT(samplers[i]);
        auto descriptorHeap = samplers[i]->GetDescriptorHeap();

        commandList->SetGraphicsRootDescriptorTable(
            rootParameterIndex,
            descriptorHeap->GetGPUDescriptorHandleForHeapStart());

        ++rootParameterIndex;
    }
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::Draw(std::uint32_t vertexCount)
{
    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(vertexCount > 0);

    BeginDraw();

    commandList->DrawInstanced(vertexCount, 1, 0, 0);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::DrawIndexed(std::uint32_t indexCount)
{
    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(indexCount > 0);

    BeginDraw();

    commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::DrawInstanced(
    std::uint32_t vertexCount,
    std::uint32_t instanceCount)
{
    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(instanceCount > 0);

    BeginDraw();

    commandList->DrawInstanced(vertexCount, instanceCount, 0, 0);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::DrawIndexedInstanced(
    std::uint32_t indexCount,
    std::uint32_t instanceCount)
{
    POMDOG_ASSERT(indexCount > 0);
    POMDOG_ASSERT(instanceCount > 0);

    BeginDraw();

    commandList->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::SetViewport(Viewport const& viewportIn)
{
    POMDOG_ASSERT(0.0f <= viewportIn.Width());
    POMDOG_ASSERT(0.0f <= viewportIn.Height());
    POMDOG_ASSERT(D3D12_VIEWPORT_BOUNDS_MAX >= viewportIn.TopLeftX() + viewportIn.Width());
    POMDOG_ASSERT(D3D12_VIEWPORT_BOUNDS_MAX >= viewportIn.TopLeftY() + viewportIn.Height());

    D3D12_VIEWPORT viewport;
    viewport.Width = static_cast<FLOAT>(viewportIn.Width());
    viewport.Height = static_cast<FLOAT>(viewportIn.Height());
    viewport.MinDepth = viewportIn.MinDepth;
    viewport.MaxDepth = viewportIn.MaxDepth;
    viewport.TopLeftX = static_cast<FLOAT>(viewportIn.TopLeftX());
    viewport.TopLeftY = static_cast<FLOAT>(viewportIn.TopLeftY());

    POMDOG_ASSERT(commandList);
    commandList->RSSetViewports(1, &viewport);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::SetScissorRectangle(Rectangle const& rectangle)
{
    std::vector<D3D12_RECT> rects;
    rects.resize(std::max<std::size_t>(1, renderTargets.size()));

    D3D12_RECT rect;
    rect.bottom = rectangle.GetBottom();
    rect.left = rectangle.GetLeft();
    rect.right = rectangle.GetRight();
    rect.top = rectangle.GetTop();

    std::fill(std::begin(rects), std::end(rects), rect);

    POMDOG_ASSERT(!rects.empty());
    POMDOG_ASSERT(commandList);
    commandList->RSSetScissorRects(rects.size(), rects.data());
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
{
    POMDOG_ASSERT(commandList);
    commandList->IASetPrimitiveTopology(
        ToPrimitiveTopology(primitiveTopology));
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::SetBlendFactor(Color const& blendFactor)
{
    POMDOG_ASSERT(commandList);
    commandList->OMSetBlendFactor(blendFactor.ToVector4().Data());
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::SetVertexBuffers(
    std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffersIn)
{
    POMDOG_ASSERT(!vertexBuffersIn.empty());

    std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferViews;
    vertexBufferViews.reserve(vertexBuffersIn.size());

    for (auto & vertexBuffer: vertexBuffersIn)
    {
        POMDOG_ASSERT(vertexBuffer);
        POMDOG_ASSERT(vertexBuffer->NativeVertexBuffer());

        auto nativeVertexBuffer = static_cast<BufferDirect3D12*>(
            vertexBuffer->NativeVertexBuffer());

        POMDOG_ASSERT(nativeVertexBuffer != nullptr);
        POMDOG_ASSERT(nativeVertexBuffer == dynamic_cast<BufferDirect3D12*>(
            vertexBuffer->NativeVertexBuffer()));

        D3D12_VERTEX_BUFFER_VIEW bufferView;
        bufferView.BufferLocation = nativeVertexBuffer->GetGPUVirtualAddress();
        bufferView.SizeInBytes = vertexBuffer->SizeInBytes();
        bufferView.StrideInBytes = vertexBuffer->StrideBytes();
        vertexBufferViews.push_back(std::move(bufferView));
    }

    POMDOG_ASSERT(commandList);
    commandList->IASetVertexBuffers(0, vertexBufferViews.size(), vertexBufferViews.data());
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::SetIndexBuffer(std::shared_ptr<IndexBuffer> const& indexBufferIn)
{
    POMDOG_ASSERT(indexBufferIn);

    auto indexBuffer = static_cast<BufferDirect3D12*>(
        indexBufferIn->NativeIndexBuffer());

    POMDOG_ASSERT(indexBuffer != nullptr);
    POMDOG_ASSERT(indexBuffer == dynamic_cast<BufferDirect3D12*>(
        indexBufferIn->NativeIndexBuffer()));

    D3D12_INDEX_BUFFER_VIEW indexBufferView;
    indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
    indexBufferView.Format = DXGIFormatHelper::ToDXGIFormat(indexBufferIn->ElementSize());
    indexBufferView.SizeInBytes = indexBufferIn->SizeInBytes();

    POMDOG_ASSERT(commandList);
    commandList->IASetIndexBuffer(&indexBufferView);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::SetSampler(int index, NativeSamplerState* samplerIn)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < D3D12_COMMONSHADER_SAMPLER_SLOT_COUNT);
    POMDOG_ASSERT(samplerIn != nullptr);

    auto sampler = static_cast<SamplerStateDirect3D12*>(samplerIn);

    POMDOG_ASSERT(sampler != nullptr);
    POMDOG_ASSERT(sampler == dynamic_cast<SamplerStateDirect3D12*>(samplerIn));

    POMDOG_ASSERT(commandList);

    POMDOG_ASSERT(!samplers.empty());
    POMDOG_ASSERT(index < static_cast<int>(samplers.size()));

    if (index < static_cast<int>(samplers.size())) {
        samplers[index] = sampler;
        needToApplyRootParameters = true;
    }
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::SetTexture(int index)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < D3D12_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
    POMDOG_ASSERT(index < static_cast<int>(textures.size()));

    textures[index] = nullptr;
    needToApplyRootParameters = true;
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::SetTexture(int index, Texture2D & textureIn)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < D3D12_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
    POMDOG_ASSERT(index < static_cast<int>(textures.size()));

    auto texture = static_cast<Texture2DDirect3D12*>(textureIn.NativeTexture2D());

    POMDOG_ASSERT(texture != nullptr);
    POMDOG_ASSERT(texture == dynamic_cast<Texture2DDirect3D12*>(textureIn.NativeTexture2D()));

    textures[index] = texture->GetShaderResourceViewHeap();
    needToApplyRootParameters = true;
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::SetTexture(int index, RenderTarget2D & textureIn)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < D3D12_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
    POMDOG_ASSERT(index < static_cast<int>(textures.size()));

    auto texture = static_cast<RenderTarget2DDirect3D12*>(textureIn.NativeRenderTarget2D());

    POMDOG_ASSERT(texture != nullptr);
    POMDOG_ASSERT(texture == dynamic_cast<RenderTarget2DDirect3D12*>(textureIn.NativeRenderTarget2D()));

    textures[index] = texture->GetShaderResourceViewHeap();
    needToApplyRootParameters = true;
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::SetRenderTarget()
{
    renderTargets.clear();
    renderTargets.push_back(backBuffer);

    auto descriptorHeap = backBuffer->GetRenderTargetDescriptorHeap();
    POMDOG_ASSERT(descriptorHeap != nullptr);

    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 1> renderTargetViews = {
        descriptorHeap->GetCPUDescriptorHandleForHeapStart() };

    auto depthStencilViewHeap = renderTargets.front()->GetDepthStencilDescriptorHeap();
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView;
    if (depthStencilViewHeap != nullptr) {
        depthStencilView = depthStencilViewHeap->GetCPUDescriptorHandleForHeapStart();
    }

    POMDOG_ASSERT(commandList);

    constexpr BOOL isSingleHandleToDescriptorRange = FALSE;

    commandList->OMSetRenderTargets(
        renderTargetViews.size(),
        renderTargetViews.data(),
        isSingleHandleToDescriptorRange,
        (depthStencilViewHeap != nullptr ? &depthStencilView : nullptr));
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::SetRenderTargets(
    std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargetsIn)
{
    POMDOG_ASSERT(!renderTargetsIn.empty());
    POMDOG_ASSERT(renderTargetsIn.size() <= D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
    POMDOG_ASSERT(renderTargetsIn.size() <= renderTargets.capacity());

    renderTargets.clear();

    std::array<D3D12_CPU_DESCRIPTOR_HANDLE,
        D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> renderTargetViews;

    POMDOG_ASSERT(renderTargetViews.size() <= D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
    POMDOG_ASSERT(renderTargetViews.size() >= renderTargetsIn.size());

    for (std::size_t i = 0; i < renderTargetsIn.size(); ++i)
    {
        POMDOG_ASSERT(renderTargetsIn[i]);

        auto renderTarget = static_cast<RenderTarget2DDirect3D12*>(renderTargetsIn[i]->NativeRenderTarget2D());
        POMDOG_ASSERT(renderTarget != nullptr);
        POMDOG_ASSERT(renderTarget == dynamic_cast<RenderTarget2DDirect3D12*>(renderTargetsIn[i]->NativeRenderTarget2D()));

        renderTargets.emplace_back(renderTargetsIn[i], renderTarget);

        auto descriptorHeap = renderTarget->GetRenderTargetDescriptorHeap();
        POMDOG_ASSERT(descriptorHeap != nullptr);

        renderTargetViews[i] = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    }

    auto depthStencilViewHeap = renderTargets.front()->GetDepthStencilDescriptorHeap();
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView;
    if (depthStencilViewHeap != nullptr) {
        depthStencilView = depthStencilViewHeap->GetCPUDescriptorHandleForHeapStart();
    }

    POMDOG_ASSERT(commandList);

    constexpr BOOL isSingleHandleToDescriptorRange = FALSE;

    commandList->OMSetRenderTargets(
        renderTargetsIn.size(),
        renderTargetViews.data(),
        isSingleHandleToDescriptorRange,
        (depthStencilViewHeap != nullptr ? &depthStencilView : nullptr));
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::SetConstantBuffers(
    std::shared_ptr<NativeConstantLayout> const& constantLayoutIn)
{
    POMDOG_ASSERT(constantLayoutIn);
    constantLayout = std::static_pointer_cast<ConstantLayoutDirect3D12>(constantLayoutIn);

    POMDOG_ASSERT(constantLayout);
    POMDOG_ASSERT(constantLayout == std::dynamic_pointer_cast<ConstantLayoutDirect3D12>(constantLayoutIn));

    needToApplyRootParameters = true;
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::SetPipelineState(
    std::shared_ptr<NativePipelineState> const& pipelineStateIn)
{
    POMDOG_ASSERT(pipelineStateIn);
    pipelineState = std::static_pointer_cast<PipelineStateDirect3D12>(pipelineStateIn);

    POMDOG_ASSERT(pipelineState);
    POMDOG_ASSERT(pipelineState == std::dynamic_pointer_cast<PipelineStateDirect3D12>(pipelineStateIn));

    POMDOG_ASSERT(pipelineState->GetPipelineState() != nullptr);

    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(pipelineState->GetPipelineState());
    POMDOG_ASSERT(pipelineState->GetRootSignature());

    commandList->SetPipelineState(pipelineState->GetPipelineState());
    commandList->SetGraphicsRootSignature(pipelineState->GetRootSignature());

    needToApplyRootParameters = true;
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D12::ResizeBackBuffers(
    ID3D12Device* device, int backBufferWidthIn, int backBufferHeightIn)
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
} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog
