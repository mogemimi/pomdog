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
namespace Win32 {

class GameWindowWin32;

} // namespace Win32
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

    GraphicsCapabilities GetCapabilities() const override;

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

    void SetPipelineState(std::shared_ptr<NativePipelineState> const& pipelineState) override;

    void SetConstantBuffers(std::shared_ptr<NativeConstantLayout> const& constantLayout) override;

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
