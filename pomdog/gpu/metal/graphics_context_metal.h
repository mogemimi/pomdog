// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/backends/graphics_context.h"
#include "pomdog/gpu/forward_declarations.h"
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include <memory>
#include <vector>

namespace pomdog::gpu::detail {
struct GraphicsCapabilities;
} // namespace pomdog::gpu::detail

namespace pomdog::gpu::detail::metal {

class GraphicsContextMetal final : public GraphicsContext {
public:
    GraphicsContextMetal() = delete;

    explicit GraphicsContextMetal(id<MTLDevice> nativeDevice);

    ~GraphicsContextMetal() override;

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

    void DispatchSemaphoreWait();

    void SetMTKView(MTKView* view);

private:
#if defined(DEBUG) && !defined(NDEBUG)
    std::vector<std::weak_ptr<Texture>> weakTextures_;
    std::vector<std::weak_ptr<RenderTarget2D>> weakRenderTargets_;
#endif
    dispatch_semaphore_t inflightSemaphore_ = nullptr;
    id<MTLCommandQueue> commandQueue_ = nullptr;
    id<MTLCommandBuffer> commandBuffer_ = nullptr;
    id<MTLRenderCommandEncoder> commandEncoder_ = nullptr;
    MTLPrimitiveType primitiveType_;
    id<MTLBuffer> indexBuffer_ = nullptr;
    MTLIndexType indexType_;
    MTKView* targetView_ = nullptr;
    bool isDrawing_ = false;
};

} // namespace pomdog::gpu::detail::metal
