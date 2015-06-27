// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GRAPHICSCONTEXTDIRECT3D11_596CF1CC_HPP
#define POMDOG_GRAPHICSCONTEXTDIRECT3D11_596CF1CC_HPP

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeGraphicsContext.hpp"
#include <wrl/client.h>
#include <memory>
#include <vector>
#include <array>

namespace Pomdog {
namespace Detail {
namespace Direct3D11 {

class PipelineStateDirect3D11;
class RenderTarget2DDirect3D11;

class GraphicsContextDirect3D11 final : public NativeGraphicsContext {
public:
    GraphicsContextDirect3D11() = delete;

    GraphicsContextDirect3D11(
        HWND windowHandle,
        Microsoft::WRL::ComPtr<IDXGIFactory1> const& dxgiFactory,
        Microsoft::WRL::ComPtr<ID3D11Device> const& nativeDevice,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContext,
        PresentationParameters const& presentationParameters);

    ~GraphicsContextDirect3D11();

    GraphicsCapabilities GetCapabilities() const override;

    void Present() override;

    void Clear(ClearOptions options, Color const& color, float depth, std::uint8_t stencil) override;

    void Draw(std::size_t vertexCount) override;

    void DrawIndexed(std::size_t indexCount) override;

    void DrawInstanced(
        std::size_t vertexCount,
        std::size_t instanceCount) override;

    void DrawIndexedInstanced(
        std::size_t indexCount,
        std::size_t instanceCount) override;

    void SetViewport(Viewport const& viewport) override;

    void SetScissorRectangle(Rectangle const& rectangle) override;

    void SetPrimitiveTopology(PrimitiveTopology primitiveTopology) override;

    void SetBlendFactor(Color const& blendFactor) override;

    void SetVertexBuffers(std::vector<VertexBufferBinding> const& vertexBuffers) override;

    void SetIndexBuffer(std::shared_ptr<IndexBuffer> const& indexBuffer) override;

    void SetPipelineState(std::shared_ptr<NativePipelineState> const& pipelineState) override;

    void SetConstantBuffer(int index, std::shared_ptr<NativeBuffer> const& constantBuffer) override;

    void SetSampler(int index, NativeSamplerState* sampler) override;

    void SetTexture(int index) override;

    void SetTexture(int index, Texture2D & texture) override;

    void SetTexture(int index, RenderTarget2D & texture) override;

    void SetRenderTarget() override;

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets) override;

    void ResizeBackBuffers(ID3D11Device* device, int backBufferWidth, int backBufferHeight);

    ID3D11DeviceContext* GetDeviceContext();

private:
    void ApplyPipelineState();

private:
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    std::vector<std::shared_ptr<RenderTarget2DDirect3D11>> renderTargets;
    static constexpr std::size_t MaxTextureCount = 8;
    std::array<ID3D11ShaderResourceView*, MaxTextureCount> textureResourceViews;
    std::shared_ptr<RenderTarget2DDirect3D11> backBuffer;
    std::shared_ptr<PipelineStateDirect3D11> pipelineState;
    std::array<FLOAT, 4> blendFactor;
    int preferredBackBufferWidth;
    int preferredBackBufferHeight;
    UINT backBufferCount;
    DXGI_FORMAT backBufferFormat;
    DepthFormat backBufferDepthFormat;
    bool needToApplyPipelineState;
};

} // namespace Direct3D11
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_GRAPHICSCONTEXTDIRECT3D11_596CF1CC_HPP
