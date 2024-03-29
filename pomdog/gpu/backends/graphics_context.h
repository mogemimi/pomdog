// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/math/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
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

    virtual void executeCommandLists(
        std::span<std::shared_ptr<CommandListImmediate>> commandLists) = 0;

    virtual void present() = 0;

    virtual void draw(
        std::uint32_t vertexCount,
        std::uint32_t startVertexLocation) = 0;

    virtual void drawIndexed(
        std::uint32_t indexCount,
        std::uint32_t startIndexLocation) = 0;

    virtual void drawInstanced(
        std::uint32_t vertexCountPerInstance,
        std::uint32_t instanceCount,
        std::uint32_t startVertexLocation,
        std::uint32_t startInstanceLocation) = 0;

    virtual void drawIndexedInstanced(
        std::uint32_t indexCountPerInstance,
        std::uint32_t instanceCount,
        std::uint32_t startIndexLocation,
        std::uint32_t startInstanceLocation) = 0;

    /// Begins a new render pass.
    virtual void beginRenderPass(const RenderPass& renderPass) = 0;

    /// Ends the current render pass.
    virtual void endRenderPass() = 0;

    /// Sets the viewport dynamically to the rasterizer stage.
    virtual void setViewport(const Viewport& viewport) = 0;

    /// Sets the scissor rectangle dynamically for a scissor test.
    virtual void setScissorRect(const Rectangle& scissorRect) = 0;

    /// Sets the constant blend color and alpha values.
    virtual void setBlendFactor(const Vector4& blendFactor) = 0;

    virtual void setVertexBuffer(
        std::uint32_t index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        std::uint32_t offset) = 0;

    virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

    virtual void setPipelineState(const std::shared_ptr<PipelineState>& pipelineState) = 0;

    virtual void setConstantBuffer(
        std::uint32_t index,
        const std::shared_ptr<Buffer>& constantBuffer,
        std::uint32_t offset,
        std::uint32_t sizeInBytes) = 0;

    virtual void setSampler(std::uint32_t index, const std::shared_ptr<SamplerState>& sampler) = 0;

    virtual void setTexture(std::uint32_t index) = 0;

    virtual void setTexture(std::uint32_t index, const std::shared_ptr<gpu::Texture2D>& texture) = 0;

    virtual void setTexture(std::uint32_t index, const std::shared_ptr<RenderTarget2D>& texture) = 0;
};

} // namespace pomdog::gpu::detail
