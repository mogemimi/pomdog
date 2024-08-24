// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/graphics_context.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
#include <array>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {
struct GraphicsCapabilities;
} // namespace pomdog::gpu::detail

namespace pomdog::gpu::detail::direct3d11 {

class DepthStencilBufferDirect3D11;
class PipelineStateDirect3D11;
class RenderTarget2DDirect3D11;

class GraphicsContextDirect3D11 final : public GraphicsContext {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        HWND windowHandle,
        const Microsoft::WRL::ComPtr<IDXGIFactory1>& dxgiFactory,
        const Microsoft::WRL::ComPtr<ID3D11Device3>& nativeDevice,
        const PresentationParameters& presentationParameters) noexcept;

    ~GraphicsContextDirect3D11();

    /// Retrieves the capabilities of a GraphicsContext.
    [[nodiscard]] GraphicsCapabilities GetCapabilities() const noexcept;

    void ExecuteCommandLists(
        std::span<std::shared_ptr<CommandListImmediate>> commandLists) override;

    void Present() override;

    void Draw(
        std::uint32_t vertexCount,
        std::uint32_t startVertexLocation) override;

    void DrawIndexed(
        std::uint32_t indexCount,
        std::uint32_t startIndexLocation) override;

    void DrawInstanced(
        std::uint32_t vertexCountPerInstance,
        std::uint32_t instanceCount,
        std::uint32_t startVertexLocation,
        std::uint32_t startInstanceLocation) override;

    void DrawIndexedInstanced(
        std::uint32_t indexCountPerInstance,
        std::uint32_t instanceCount,
        std::uint32_t startIndexLocation,
        std::uint32_t startInstanceLocation) override;

    void BeginRenderPass(const RenderPass& renderPass) override;

    void EndRenderPass() override;

    void SetViewport(const Viewport& viewport) override;

    void SetScissorRect(const Rectangle& scissorRect) override;

    void SetBlendFactor(const Vector4& blendFactor) override;

    void SetVertexBuffer(
        std::uint32_t index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        std::uint32_t offset) override;

    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    void SetPipelineState(const std::shared_ptr<PipelineState>& pipelineState) override;

    void SetConstantBuffer(
        std::uint32_t index,
        const std::shared_ptr<Buffer>& constantBuffer,
        std::uint32_t offset,
        std::uint32_t sizeInBytes) override;

    void SetSampler(std::uint32_t index, const std::shared_ptr<SamplerState>& sampler) override;

    void SetTexture(std::uint32_t index) override;

    void SetTexture(std::uint32_t index, const std::shared_ptr<gpu::Texture2D>& texture) override;

    void SetTexture(std::uint32_t index, const std::shared_ptr<RenderTarget2D>& texture) override;

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
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    std::vector<std::weak_ptr<Texture>> weakTextures;
    std::vector<std::weak_ptr<RenderTarget2D>> weakRenderTargets;
#endif
    static constexpr std::size_t MaxTextureCount = 8;
    std::array<ID3D11ShaderResourceView*, MaxTextureCount> textureResourceViews;
    std::shared_ptr<RenderTarget2DDirect3D11> backBuffer;
    std::shared_ptr<DepthStencilBufferDirect3D11> backBufferDepthStencil;
    std::shared_ptr<PipelineStateDirect3D11> pipelineState;
    std::array<FLOAT, 4> blendFactor = {{1.0f, 1.0f, 1.0f, 1.0f}};
    int preferredBackBufferWidth = 1;
    int preferredBackBufferHeight = 1;
    UINT backBufferCount = 2;
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_UNKNOWN;
    PixelFormat backBufferDepthFormat = PixelFormat::Invalid;
    bool needToApplyPipelineState = true;
};

} // namespace pomdog::gpu::detail::direct3d11
