// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/memory/linear_page_allocator.h"
#include "pomdog/memory/unsafe_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {
class GraphicsContext;
} // namespace pomdog::gpu::detail

namespace pomdog::gpu::detail {

/// GraphicsCommand is the abstract base for a single recorded GPU command.
class GraphicsCommand {
public:
    virtual ~GraphicsCommand();

    virtual void execute(GraphicsContext& graphicsContext) const = 0;
};

/// CommandListImmediate records GPU commands into a linear allocator for
/// deferred execution by a GraphicsContext backend.
class CommandListImmediate final : public CommandList {
private:
    pomdog::memory::LinearPageAllocator allocator_;
    std::vector<unsafe_ptr<const GraphicsCommand>> commands_;
    std::vector<unsafe_ptr<const GraphicsCommand>> renderPassCommands_;
    bool needToEndRenderPass_ = false;

public:
    CommandListImmediate() noexcept;
    CommandListImmediate(const CommandListImmediate&) = delete;
    CommandListImmediate& operator=(const CommandListImmediate&) = delete;

    CommandListImmediate(CommandListImmediate&&) = delete;
    CommandListImmediate& operator=(CommandListImmediate&&) = delete;

    ~CommandListImmediate() override;

    /// Declares that recording to the command list is completed.
    void close() override;

    /// Clears the graphics commands.
    void reset() override;

    /// Draws the specified non-indexed primitives.
    ///
    /// `vertexCount` is the number of vertices to draw; `startVertexLocation`
    /// is the index of the first vertex in the vertex buffer.
    void draw(
        u32 vertexCount,
        u32 startVertexLocation) override;

    /// Draws the specified indexed primitives.
    ///
    /// `indexCount` is the number of indices to draw; `startIndexLocation`
    /// is the index of the first index in the index buffer.
    void drawIndexed(
        u32 indexCount,
        u32 startIndexLocation) override;

    /// Draws the specified instanced primitives.
    void drawInstanced(
        u32 vertexCountPerInstance,
        u32 instanceCount,
        u32 startVertexLocation,
        u32 startInstanceLocation) override;

    /// Draws the specified indexed, instanced primitives.
    void drawIndexedInstanced(
        u32 indexCountPerInstance,
        u32 instanceCount,
        u32 startIndexLocation,
        u32 startInstanceLocation) override;

    /// Begins a new render pass.
    void beginRenderPass(RenderPass&& renderPass) override;

    /// Ends the current render pass.
    void endRenderPass() override;

    /// Sets the viewport dynamically to the rasterizer stage.
    void setViewport(const Viewport& viewport) override;

    /// Sets the scissor rectangle dynamically for a scissor test.
    void setScissorRect(const Rect2D& scissorRect) override;

    /// Sets the constant blend color and alpha values.
    void setBlendFactor(const Vector4& blendFactor) override;

    /// Sets a vertex buffer.
    void setVertexBuffer(u32 index, const std::shared_ptr<VertexBuffer>& vertexBuffer) override;

    /// Sets a vertex buffer.
    void setVertexBuffer(
        u32 index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        u32 offset) override;

    /// Sets an index buffer.
    void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    /// Sets a pipeline state.
    void setPipelineState(const std::shared_ptr<PipelineState>& pipelineState) override;

    /// Sets a constant buffer.
    void setConstantBuffer(u32 index, const std::shared_ptr<ConstantBuffer>& constantBuffer) override;

    /// Sets a constant buffer.
    void setConstantBuffer(
        u32 index,
        const std::shared_ptr<ConstantBuffer>& constantBuffer,
        u32 offset) override;

    /// Sets an empty texture to the specified slot.
    void setTexture(u32 index) override;

    /// Sets a texture to the specified slot.
    void setTexture(u32 index, const std::shared_ptr<gpu::Texture>& texture) override;

    /// Sets a sampler state to the specified slot.
    void setSamplerState(u32 index, const std::shared_ptr<SamplerState>& samplerState) override;

    /// Executes all recorded commands immediately against `graphicsContext`.
    void executeImmediate(GraphicsContext& graphicsContext);

private:
    void flushRenderPassCommands();
};

} // namespace pomdog::gpu::detail
