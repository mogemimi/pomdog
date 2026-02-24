// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/backends/graphics_context.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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

    void setScissorRect(const Rectangle& scissorRect) override;

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

    void setTexture(u32 index, const std::shared_ptr<gpu::Texture2D>& texture) override;

    void setTexture(u32 index, const std::shared_ptr<RenderTarget2D>& texture) override;

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
