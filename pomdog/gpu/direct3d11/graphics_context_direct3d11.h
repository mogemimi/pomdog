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
    initialize(
        HWND windowHandle,
        const Microsoft::WRL::ComPtr<IDXGIFactory1>& dxgiFactory,
        const Microsoft::WRL::ComPtr<ID3D11Device3>& nativeDevice,
        const PresentationParameters& presentationParameters) noexcept;

    ~GraphicsContextDirect3D11();

    /// Retrieves the capabilities of a GraphicsContext.
    [[nodiscard]] GraphicsCapabilities
    getCapabilities() const noexcept;

    void executeCommandLists(
        std::span<std::shared_ptr<CommandListImmediate>> commandLists) override;

    void present() override;

    void draw(
        std::uint32_t vertexCount,
        std::uint32_t startVertexLocation) override;

    void drawIndexed(
        std::uint32_t indexCount,
        std::uint32_t startIndexLocation) override;

    void drawInstanced(
        std::uint32_t vertexCountPerInstance,
        std::uint32_t instanceCount,
        std::uint32_t startVertexLocation,
        std::uint32_t startInstanceLocation) override;

    void drawIndexedInstanced(
        std::uint32_t indexCountPerInstance,
        std::uint32_t instanceCount,
        std::uint32_t startIndexLocation,
        std::uint32_t startInstanceLocation) override;

    void beginRenderPass(const RenderPass& renderPass) override;

    void endRenderPass() override;

    void setViewport(const Viewport& viewport) override;

    void setScissorRect(const Rectangle& scissorRect) override;

    void setBlendFactor(const Vector4& blendFactor) override;

    void setVertexBuffer(
        std::uint32_t index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        std::uint32_t offset) override;

    void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    void setPipelineState(const std::shared_ptr<PipelineState>& pipelineState) override;

    void setConstantBuffer(
        std::uint32_t index,
        const std::shared_ptr<Buffer>& constantBuffer,
        std::uint32_t offset,
        std::uint32_t sizeInBytes) override;

    void setSampler(std::uint32_t index, const std::shared_ptr<SamplerState>& sampler) override;

    void setTexture(std::uint32_t index) override;

    void setTexture(std::uint32_t index, const std::shared_ptr<gpu::Texture2D>& texture) override;

    void setTexture(std::uint32_t index, const std::shared_ptr<RenderTarget2D>& texture) override;

    [[nodiscard]] std::unique_ptr<Error>
    resizeBackBuffers(ID3D11Device* device, int backBufferWidth, int backBufferHeight) noexcept;

    /// Gets the pointer of the immediate graphics context.
    [[nodiscard]] ID3D11DeviceContext3*
    getImmediateContext() noexcept;

    /// Gets the pointer of the deferred graphics context.
    [[nodiscard]] ID3D11DeviceContext3*
    getDeferredContext() noexcept;

private:
    void applyPipelineState();

private:
    Microsoft::WRL::ComPtr<ID3D11DeviceContext3> immediateContext_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext3> deferredContext_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
    std::vector<std::shared_ptr<RenderTarget2DDirect3D11>> renderTargets_;
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    std::vector<std::weak_ptr<Texture>> weakTextures_;
    std::vector<std::weak_ptr<RenderTarget2D>> weakRenderTargets_;
#endif
    static constexpr std::size_t MaxTextureCount = 8;
    std::array<ID3D11ShaderResourceView*, MaxTextureCount> textureResourceViews_;
    std::shared_ptr<RenderTarget2DDirect3D11> backBuffer_;
    std::shared_ptr<DepthStencilBufferDirect3D11> backBufferDepthStencil_;
    std::shared_ptr<PipelineStateDirect3D11> pipelineState_;
    std::array<FLOAT, 4> blendFactor_ = {1.0f, 1.0f, 1.0f, 1.0f};
    int preferredBackBufferWidth_ = 1;
    int preferredBackBufferHeight_ = 1;
    UINT backBufferCount_ = 2;
    DXGI_FORMAT backBufferFormat_ = DXGI_FORMAT_UNKNOWN;
    PixelFormat backBufferDepthFormat_ = PixelFormat::Invalid;
    bool needToApplyPipelineState_ = true;
};

} // namespace pomdog::gpu::detail::direct3d11
