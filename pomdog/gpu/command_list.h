// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/math/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdlib>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

class POMDOG_EXPORT CommandList {
public:
    CommandList() noexcept;
    CommandList(const CommandList&) = delete;
    CommandList& operator=(const CommandList&) = delete;

    virtual ~CommandList();

    /// Declares that recording to the command list is completed.
    virtual void Close() = 0;

    /// Clears the graphics commands.
    virtual void Reset() = 0;

    /// Draws the specified non-indexed primitives.
    ///
    /// @param vertexCount Number of vertices to draw.
    /// @param startVertexLocation Index of the first vertex to draw.
    virtual void Draw(
        std::size_t vertexCount,
        std::size_t startVertexLocation) = 0;

    /// Draws the specified indexed primitives.
    virtual void DrawIndexed(
        std::size_t indexCount,
        std::size_t startIndexLocation) = 0;

    /// Draws the specified instanced primitives.
    virtual void DrawInstanced(
        std::size_t vertexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startVertexLocation,
        std::size_t startInstanceLocation) = 0;

    /// Draws the specified indexed, instanced primitives.
    virtual void DrawIndexedInstanced(
        std::size_t indexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startIndexLocation,
        std::size_t startInstanceLocation) = 0;

    /// Sets a group of render targets.
    virtual void SetRenderPass(RenderPass&& renderPass) = 0;

    /// Sets the viewport dynamically to the rasterizer stage.
    virtual void SetViewport(const Viewport& viewport) = 0;

    /// Sets the scissor rectangle dynamically for a scissor test.
    virtual void SetScissorRect(const Rectangle& scissorRect) = 0;

    /// Sets the constant blend color and alpha values.
    virtual void SetBlendFactor(const Vector4& blendFactor) = 0;

    /// Sets a vertex buffer.
    virtual void SetVertexBuffer(int index, const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;

    /// Sets a vertex buffer.
    virtual void SetVertexBuffer(
        int index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        std::size_t offset) = 0;

    /// Sets an index buffer.
    virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

    /// Sets a pipeline state.
    virtual void SetPipelineState(const std::shared_ptr<PipelineState>& pipelineState) = 0;

    /// Sets a constant buffer.
    virtual void SetConstantBuffer(int index, const std::shared_ptr<ConstantBuffer>& constantBuffer) = 0;

    /// Sets a constant buffer.
    virtual void SetConstantBuffer(
        int index,
        const std::shared_ptr<ConstantBuffer>& constantBuffer,
        std::size_t offset) = 0;

    /// Sets an empty texture to the specified slot.
    virtual void SetTexture(int index) = 0;

    /// Sets a texture to the specified slot.
    virtual void SetTexture(int index, const std::shared_ptr<gpu::Texture2D>& texture) = 0;

    /// Sets a texture to the specified slot.
    virtual void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture) = 0;

    /// Sets a sampler state to the specified slot.
    virtual void SetSamplerState(int index, const std::shared_ptr<SamplerState>& samplerState) = 0;
};

} // namespace pomdog::gpu
