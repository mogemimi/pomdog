// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/backends/graphics_context.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class Texture;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail {
struct GraphicsCapabilities;
} // namespace pomdog::gpu::detail

namespace pomdog::gpu::detail::metal {

/// GraphicsContextMetal is the Metal implementation of GraphicsContext.
class GraphicsContextMetal final : public GraphicsContext {
private:
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    std::vector<std::weak_ptr<Texture>> weakTextures_;
    std::vector<std::weak_ptr<Texture>> weakRenderTargets_;
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

public:
    GraphicsContextMetal() = delete;

    explicit GraphicsContextMetal(id<MTLDevice> nativeDevice);

    ~GraphicsContextMetal() override;

    /// Retrieves the capabilities of a GraphicsContext.
    [[nodiscard]] GraphicsCapabilities
    getCapabilities() const noexcept;

    void executeCommandLists(
        std::span<std::shared_ptr<CommandListImmediate>> commandLists) override;

    /// Commits the current command buffer and presents it to the MTKView's drawable.
    void present() override;

    /// Draws non-indexed primitives.
    void draw(
        u32 vertexCount,
        u32 startVertexLocation) override;

    /// Draws indexed primitives.
    void drawIndexed(
        u32 indexCount,
        u32 startIndexLocation) override;

    /// Draws instanced non-indexed primitives.
    void drawInstanced(
        u32 vertexCountPerInstance,
        u32 instanceCount,
        u32 startVertexLocation,
        u32 startInstanceLocation) override;

    /// Draws instanced indexed primitives.
    void drawIndexedInstanced(
        u32 indexCountPerInstance,
        u32 instanceCount,
        u32 startIndexLocation,
        u32 startInstanceLocation) override;

    /// Begins a render pass with the specified targets and clear values.
    void beginRenderPass(const RenderPass& renderPass) override;

    /// Ends the current render pass.
    void endRenderPass() override;

    /// Sets the viewport for the rasterizer stage.
    void setViewport(const Viewport& viewport) override;

    /// Sets the scissor rectangle for the rasterizer stage.
    void setScissorRect(const Rect2D& scissorRect) override;

    /// Sets the constant blend factor used by blend operations that reference it.
    void setBlendFactor(const Vector4& blendFactor) override;

    /// Binds a vertex buffer to the given input slot.
    void setVertexBuffer(
        u32 index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        u32 offset) override;

    /// Binds an index buffer.
    void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    /// Binds a pipeline state object.
    void setPipelineState(const std::shared_ptr<PipelineState>& pipelineState) override;

    /// Binds a constant buffer to the given slot.
    void setConstantBuffer(
        u32 index,
        const std::shared_ptr<Buffer>& constantBuffer,
        u32 offset,
        u32 sizeInBytes) override;

    /// Binds a sampler state to the given texture unit.
    void setSampler(u32 index, const std::shared_ptr<SamplerState>& sampler) override;

    /// Unbinds the texture at the given slot.
    void setTexture(u32 index) override;

    /// Binds a texture to the given slot.
    void setTexture(u32 index, const std::shared_ptr<gpu::Texture>& texture) override;

    /// Waits on the in-flight semaphore to ensure no more than 3 frames are simultaneously in flight.
    void dispatchSemaphoreWait();

    /// Sets the MTKView that receives the rendered output.
    void setMTKView(MTKView* view);
};

} // namespace pomdog::gpu::detail::metal
