// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Rectangle;
class Vector4;
} // namespace pomdog

namespace pomdog::gpu {
class ConstantBuffer;
class IndexBuffer;
class PipelineState;
class RenderTarget2D;
class SamplerState;
class Texture2D;
class VertexBuffer;
class Viewport;
struct RenderPass;
} // namespace pomdog::gpu

namespace pomdog::gpu {

class POMDOG_EXPORT CommandList {
public:
    CommandList() noexcept;
    CommandList(const CommandList&) = delete;
    CommandList& operator=(const CommandList&) = delete;

    virtual ~CommandList();

    /// Declares that recording to the command list is completed.
    virtual void
    close() = 0;

    /// Clears the graphics commands.
    virtual void
    reset() = 0;

    /// Draws the specified non-indexed primitives.
    ///
    /// @param vertexCount Number of vertices to draw.
    /// @param startVertexLocation Index of the first vertex to draw.
    virtual void
    draw(
        u32 vertexCount,
        u32 startVertexLocation) = 0;

    /// Draws the specified indexed primitives.
    virtual void
    drawIndexed(
        u32 indexCount,
        u32 startIndexLocation) = 0;

    /// Draws the specified instanced primitives.
    virtual void
    drawInstanced(
        u32 vertexCountPerInstance,
        u32 instanceCount,
        u32 startVertexLocation,
        u32 startInstanceLocation) = 0;

    /// Draws the specified indexed, instanced primitives.
    virtual void
    drawIndexedInstanced(
        u32 indexCountPerInstance,
        u32 instanceCount,
        u32 startIndexLocation,
        u32 startInstanceLocation) = 0;

    /// Sets a group of render targets.
    virtual void
    setRenderPass(RenderPass&& renderPass) = 0;

    /// Sets the viewport dynamically to the rasterizer stage.
    virtual void
    setViewport(const Viewport& viewport) = 0;

    /// Sets the scissor rectangle dynamically for a scissor test.
    virtual void
    setScissorRect(const Rectangle& scissorRect) = 0;

    /// Sets the constant blend color and alpha values.
    virtual void
    setBlendFactor(const Vector4& blendFactor) = 0;

    /// Sets a vertex buffer.
    virtual void
    setVertexBuffer(u32 index, const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;

    /// Sets a vertex buffer.
    virtual void
    setVertexBuffer(
        u32 index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        u32 offset) = 0;

    /// Sets an index buffer.
    virtual void
    setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

    /// Sets a pipeline state.
    virtual void
    setPipelineState(const std::shared_ptr<PipelineState>& pipelineState) = 0;

    /// Sets a constant buffer.
    virtual void
    setConstantBuffer(u32 index, const std::shared_ptr<ConstantBuffer>& constantBuffer) = 0;

    /// Sets a constant buffer.
    virtual void
    setConstantBuffer(
        u32 index,
        const std::shared_ptr<ConstantBuffer>& constantBuffer,
        u32 offset) = 0;

    /// Sets an empty texture to the specified slot.
    virtual void
    setTexture(u32 index) = 0;

    /// Sets a texture to the specified slot.
    virtual void
    setTexture(u32 index, const std::shared_ptr<Texture2D>& texture) = 0;

    /// Sets a texture to the specified slot.
    virtual void
    setTexture(u32 index, const std::shared_ptr<RenderTarget2D>& texture) = 0;

    /// Sets a sampler state to the specified slot.
    virtual void
    setSamplerState(u32 index, const std::shared_ptr<SamplerState>& samplerState) = 0;
};

} // namespace pomdog::gpu
