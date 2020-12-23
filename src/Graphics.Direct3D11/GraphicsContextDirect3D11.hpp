// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "../Graphics.Backends/NativeGraphicsContext.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <wrl/client.h>
#include <array>
#include <memory>
#include <vector>

namespace Pomdog::Detail::Direct3D11 {

class DepthStencilBufferDirect3D11;
class PipelineStateDirect3D11;
class RenderTarget2DDirect3D11;

class GraphicsContextDirect3D11 final : public NativeGraphicsContext {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        HWND windowHandle,
        const Microsoft::WRL::ComPtr<IDXGIFactory1>& dxgiFactory,
        const Microsoft::WRL::ComPtr<ID3D11Device3>& nativeDevice,
        const PresentationParameters& presentationParameters) noexcept;

    ~GraphicsContextDirect3D11();

    GraphicsCapabilities GetCapabilities() const override;

    void ExecuteCommandLists(
        const std::vector<std::shared_ptr<GraphicsCommandListImmediate>>& commandLists) override;

    void Present() override;

    void Draw(
        std::size_t vertexCount,
        std::size_t startVertexLocation) override;

    void DrawIndexed(
        std::size_t indexCount,
        std::size_t startIndexLocation) override;

    void DrawInstanced(
        std::size_t vertexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startVertexLocation,
        std::size_t startInstanceLocation) override;

    void DrawIndexedInstanced(
        std::size_t indexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startIndexLocation,
        std::size_t startInstanceLocation) override;

    void SetRenderPass(const RenderPass& renderPass) override;

    void SetViewport(const Viewport& viewport) override;

    void SetScissorRect(const Rectangle& scissorRect) override;

    void SetBlendFactor(const Vector4& blendFactor) override;

    void SetVertexBuffer(
        int index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        std::size_t offset) override;

    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    void SetPipelineState(const std::shared_ptr<PipelineState>& pipelineState) override;

    void SetConstantBuffer(
        int index,
        const std::shared_ptr<NativeBuffer>& constantBuffer,
        std::size_t offset,
        std::size_t sizeInBytes) override;

    void SetSampler(int index, const std::shared_ptr<SamplerState>& sampler) override;

    void SetTexture(int index) override;

    void SetTexture(int index, const std::shared_ptr<Texture2D>& texture) override;

    void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture) override;

    [[nodiscard]] std::unique_ptr<Error>
    ResizeBackBuffers(ID3D11Device* device, int backBufferWidth, int backBufferHeight) noexcept;

    /// Gets the pointer of the immediate graphics context.
    [[nodiscard]] ID3D11DeviceContext3* GetImmediateContext() noexcept;

    /// Gets the pointer of the deferred graphics context.
    [[nodiscard]] ID3D11DeviceContext3* GetDeferredContext() noexcept;

private:
    void ApplyPipelineState();

private:
    Microsoft::WRL::ComPtr<ID3D11DeviceContext3> immediateContext;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext3> deferredContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    std::vector<std::shared_ptr<RenderTarget2DDirect3D11>> renderTargets;
#if defined(DEBUG) && !defined(NDEBUG)
    std::vector<std::weak_ptr<Texture>> weakTextures;
    std::vector<std::weak_ptr<RenderTarget2D>> weakRenderTargets;
#endif
    static constexpr std::size_t MaxTextureCount = 8;
    std::array<ID3D11ShaderResourceView*, MaxTextureCount> textureResourceViews;
    std::shared_ptr<RenderTarget2DDirect3D11> backBuffer;
    std::shared_ptr<DepthStencilBufferDirect3D11> backBufferDepthStencil;
    std::shared_ptr<PipelineStateDirect3D11> pipelineState;
    std::array<FLOAT, 4> blendFactor = {1.0f, 1.0f, 1.0f, 1.0f};
    int preferredBackBufferWidth = 1;
    int preferredBackBufferHeight = 1;
    UINT backBufferCount = 2;
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_UNKNOWN;
    SurfaceFormat backBufferDepthFormat = SurfaceFormat::Invalid;
    bool needToApplyPipelineState = true;
};

} // namespace Pomdog::Detail::Direct3D11
