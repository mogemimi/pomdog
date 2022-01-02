// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/math/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {

class CommandListImmediate;

class GraphicsContext {
public:
    GraphicsContext() noexcept;
    GraphicsContext(const GraphicsContext&) = delete;
    GraphicsContext& operator=(const GraphicsContext&) = delete;

    virtual ~GraphicsContext();

    virtual void ExecuteCommandLists(
        std::span<std::shared_ptr<CommandListImmediate>> commandLists) = 0;

    virtual void Present() = 0;

    virtual void Draw(
        std::size_t vertexCount,
        std::size_t startVertexLocation) = 0;

    virtual void DrawIndexed(
        std::size_t indexCount,
        std::size_t startIndexLocation) = 0;

    virtual void DrawInstanced(
        std::size_t vertexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startVertexLocation,
        std::size_t startInstanceLocation) = 0;

    virtual void DrawIndexedInstanced(
        std::size_t indexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startIndexLocation,
        std::size_t startInstanceLocation) = 0;

    /// Begins a new render pass.
    virtual void BeginRenderPass(const RenderPass& renderPass) = 0;

    /// Ends the current render pass.
    virtual void EndRenderPass() = 0;

    /// Sets the viewport dynamically to the rasterizer stage.
    virtual void SetViewport(const Viewport& viewport) = 0;

    /// Sets the scissor rectangle dynamically for a scissor test.
    virtual void SetScissorRect(const Rectangle& scissorRect) = 0;

    /// Sets the constant blend color and alpha values.
    virtual void SetBlendFactor(const Vector4& blendFactor) = 0;

    virtual void SetVertexBuffer(
        int index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        std::size_t offset) = 0;

    virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

    virtual void SetPipelineState(const std::shared_ptr<PipelineState>& pipelineState) = 0;

    virtual void SetConstantBuffer(
        int index,
        const std::shared_ptr<Buffer>& constantBuffer,
        std::size_t offset,
        std::size_t sizeInBytes) = 0;

    virtual void SetSampler(int index, const std::shared_ptr<SamplerState>& sampler) = 0;

    virtual void SetTexture(int index) = 0;

    virtual void SetTexture(int index, const std::shared_ptr<gpu::Texture2D>& texture) = 0;

    virtual void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture) = 0;
};

} // namespace pomdog::gpu::detail
