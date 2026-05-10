// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Rect2D;
class Vector4;
} // namespace pomdog

namespace pomdog::gpu {
class Buffer;
class IndexBuffer;
class PipelineState;
class SamplerState;
class Texture;
class VertexBuffer;
class Viewport;
struct RenderPass;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail {
class CommandListImmediate;
} // namespace pomdog::gpu::detail

namespace pomdog::gpu::detail {

/// GraphicsContext is the backend interface that executes recorded GPU commands
/// against a concrete graphics API (D3D11, GL4, Metal).
class GraphicsContext {
public:
    GraphicsContext() noexcept;
    GraphicsContext(const GraphicsContext&) = delete;
    GraphicsContext& operator=(const GraphicsContext&) = delete;

    virtual ~GraphicsContext();

    /// Executes all recorded command lists against the underlying graphics API.
    virtual void executeCommandLists(
        std::span<std::shared_ptr<CommandListImmediate>> commandLists) = 0;

    /// Presents the rendered back buffer to the display.
    virtual void present() = 0;

    /// Draws the specified non-indexed primitives.
    virtual void draw(
        u32 vertexCount,
        u32 startVertexLocation) = 0;

    /// Draws the specified indexed primitives.
    virtual void drawIndexed(
        u32 indexCount,
        u32 startIndexLocation) = 0;

    /// Draws the specified instanced primitives.
    virtual void drawInstanced(
        u32 vertexCountPerInstance,
        u32 instanceCount,
        u32 startVertexLocation,
        u32 startInstanceLocation) = 0;

    /// Draws the specified indexed, instanced primitives.
    virtual void drawIndexedInstanced(
        u32 indexCountPerInstance,
        u32 instanceCount,
        u32 startIndexLocation,
        u32 startInstanceLocation) = 0;

    /// Begins a new render pass.
    virtual void beginRenderPass(const RenderPass& renderPass) = 0;

    /// Ends the current render pass.
    virtual void endRenderPass() = 0;

    /// Sets the viewport dynamically to the rasterizer stage.
    virtual void setViewport(const Viewport& viewport) = 0;

    /// Sets the scissor rectangle dynamically for a scissor test.
    virtual void setScissorRect(const Rect2D& scissorRect) = 0;

    /// Sets the constant blend color and alpha values.
    virtual void setBlendFactor(const Vector4& blendFactor) = 0;

    /// Binds a vertex buffer to the specified input slot.
    virtual void setVertexBuffer(
        u32 index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        u32 offset) = 0;

    /// Binds an index buffer.
    virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

    /// Binds a pipeline state object.
    virtual void setPipelineState(const std::shared_ptr<PipelineState>& pipelineState) = 0;

    /// Binds a constant (uniform) buffer to the specified slot.
    virtual void setConstantBuffer(
        u32 index,
        const std::shared_ptr<Buffer>& constantBuffer,
        u32 offset,
        u32 sizeInBytes) = 0;

    /// Binds a sampler state to the specified sampler slot.
    virtual void setSampler(u32 index, const std::shared_ptr<SamplerState>& sampler) = 0;

    /// Unbinds the texture at the specified slot, replacing it with a null binding.
    virtual void setTexture(u32 index) = 0;

    /// Binds a texture to the specified shader resource slot.
    virtual void setTexture(u32 index, const std::shared_ptr<gpu::Texture>& texture) = 0;
};

} // namespace pomdog::gpu::detail
