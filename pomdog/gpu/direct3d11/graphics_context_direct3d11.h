// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/backends/graphics_context.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
#include <array>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class Texture;
struct PresentationParameters;
} // namespace pomdog::gpu

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

    /// Returns true if V-Sync (display sync) is currently enabled.
    [[nodiscard]] bool
    getDisplaySyncEnabled() const noexcept;

    /// Enables or disables V-Sync (display sync).
    ///
    /// When enabled the DXGI swap chain presents with sync interval 1
    /// (wait for the next vertical blank).  When disabled it presents
    /// with sync interval 0 (present immediately).
    void
    setDisplaySyncEnabled(bool enabled) noexcept;

    void draw(
        u32 vertexCount,
        u32 startVertexLocation) override;

    void drawIndexed(
        u32 indexCount,
        u32 startIndexLocation) override;

    void drawInstanced(
        u32 vertexCountPerInstance,
        u32 instanceCount,
        u32 startVertexLocation,
        u32 startInstanceLocation) override;

    void drawIndexedInstanced(
        u32 indexCountPerInstance,
        u32 instanceCount,
        u32 startIndexLocation,
        u32 startInstanceLocation) override;

    void beginRenderPass(const RenderPass& renderPass) override;

    void endRenderPass() override;

    void setViewport(const Viewport& viewport) override;

    void setScissorRect(const Rect2D& scissorRect) override;

    void setBlendFactor(const Vector4& blendFactor) override;

    void setVertexBuffer(
        u32 index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        u32 offset) override;

    void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    void setPipelineState(const std::shared_ptr<PipelineState>& pipelineState) override;

    void setConstantBuffer(
        u32 index,
        const std::shared_ptr<Buffer>& constantBuffer,
        u32 offset,
        u32 sizeInBytes) override;

    void setSampler(u32 index, const std::shared_ptr<SamplerState>& sampler) override;

    void setTexture(u32 index) override;

    void setTexture(u32 index, const std::shared_ptr<gpu::Texture>& texture) override;

    [[nodiscard]] std::unique_ptr<Error>
    resizeBackBuffers(ID3D11Device* device, int backBufferWidth, int backBufferHeight) noexcept;

    /// Gets the pointer of the immediate graphics context.
    [[nodiscard]] unsafe_ptr<ID3D11DeviceContext3>
    getImmediateContext() noexcept;

    /// Gets the pointer of the deferred graphics context.
    [[nodiscard]] unsafe_ptr<ID3D11DeviceContext3>
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
    std::vector<std::weak_ptr<Texture>> weakRenderTargets_;
#endif
    static constexpr std::size_t MaxTextureCount = 8;
    std::array<ID3D11ShaderResourceView*, MaxTextureCount> textureResourceViews_;
    std::shared_ptr<RenderTarget2DDirect3D11> backBuffer_;
    std::shared_ptr<DepthStencilBufferDirect3D11> backBufferDepthStencil_;
    std::shared_ptr<PipelineStateDirect3D11> pipelineState_;
    std::array<FLOAT, 4> blendFactor_ = {{1.0f, 1.0f, 1.0f, 1.0f}};
    i32 preferredBackBufferWidth_ = 1;
    i32 preferredBackBufferHeight_ = 1;
    i32 syncInterval_ = 0;
    UINT backBufferCount_ = 2;
    DXGI_FORMAT backBufferFormat_ = DXGI_FORMAT_UNKNOWN;
    PixelFormat backBufferDepthFormat_ = PixelFormat::Invalid;
    bool needToApplyPipelineState_ = true;
};

} // namespace pomdog::gpu::detail::direct3d11
