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

    void dispatchSemaphoreWait();

    void setMTKView(MTKView* view);

private:
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
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
