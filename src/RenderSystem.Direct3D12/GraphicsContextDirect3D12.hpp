// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GRAPHICSCONTEXTDIRECT3D12_625E64CB_HPP
#define POMDOG_GRAPHICSCONTEXTDIRECT3D12_625E64CB_HPP

#include "../RenderSystem/NativeGraphicsContext.hpp"
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <memory>
#include <vector>
#include <array>

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {

class ConstantLayoutDirect3D12;
class PipelineStateDirect3D12;
class RenderTarget2DDirect3D12;
class SamplerStateDirect3D12;
class Texture2DDirect3D12;

class GraphicsContextDirect3D12 final : public NativeGraphicsContext {
public:
    GraphicsContextDirect3D12(
        Microsoft::WRL::ComPtr<ID3D12Device> const& device,
        HWND windowHandle,
        PresentationParameters const& presentationParameters);

    ~GraphicsContextDirect3D12();

    GraphicsCapabilities GetCapabilities() const override;

    void Clear(ClearOptions options, Color const& color, float depth, std::uint8_t stencil) override;

    void Present() override;

    void Draw(std::uint32_t vertexCount) override;

    void DrawIndexed(std::uint32_t indexCount) override;

    void DrawInstanced(
        std::uint32_t vertexCount,
        std::uint32_t instanceCount) override;

    void DrawIndexedInstanced(
        std::uint32_t indexCount,
        std::uint32_t instanceCount) override;

    void SetViewport(Viewport const& viewport) override;

    void SetScissorRectangle(Rectangle const& rectangle) override;

    void SetPrimitiveTopology(PrimitiveTopology primitiveTopology) override;

    void SetBlendFactor(Color const& blendFactor) override;

    void SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers) override;

    void SetIndexBuffer(std::shared_ptr<IndexBuffer> const& indexBuffer) override;

    void SetSampler(int index, NativeSamplerState* sampler) override;

    void SetTexture(int index) override;

    void SetTexture(int index, Texture2D & texture) override;

    void SetTexture(int index, RenderTarget2D & texture) override;

    void SetRenderTarget() override;

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets) override;

    void SetConstantBuffers(std::shared_ptr<NativeConstantLayout> const& constantLayout) override;

    void SetPipelineState(std::shared_ptr<NativePipelineState> const& pipelineState) override;

    void ResizeBackBuffers(ID3D12Device* device, int backBufferWidth, int backBufferHeight);

    ///@todo FIXME: Bad naming and bad API
    void BackBufferBarrier();

private:
    void BeginDraw();

private:
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    HANDLE fenceEvent;

    std::shared_ptr<RenderTarget2DDirect3D12> backBuffer;
    std::shared_ptr<PipelineStateDirect3D12> pipelineState;
    std::shared_ptr<ConstantLayoutDirect3D12> constantLayout;

    std::vector<std::shared_ptr<RenderTarget2DDirect3D12>> renderTargets;

    static constexpr std::size_t MaxTextureCount = 8;

    ///@todo Replace with std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>>
    std::array<ID3D12DescriptorHeap*, MaxTextureCount> textures;

    static constexpr std::size_t MaxSamplerCount = D3D12_COMMONSHADER_SAMPLER_SLOT_COUNT;

    ///@todo Replace with std::vector<std::shared_ptr<SamplerStateDirect3D12>>
    std::array<SamplerStateDirect3D12*, MaxSamplerCount> samplers;

    int preferredBackBufferWidth;
    int preferredBackBufferHeight;
    DXGI_FORMAT backBufferFormat;
    DepthFormat backBufferDepthFormat;
    static constexpr UINT backBufferCount = 2;

    bool needToApplyRootParameters;
};

} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_GRAPHICSCONTEXTDIRECT3D12_625E64CB_HPP
