// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vulkan/vulkan.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {

class CommandListVulkan final : public CommandList {
public:
    CommandListVulkan();

    /// Declares that recording to the command list is completed.
    void close() override;

    /// Clears the graphics commands.
    void reset() override;

    /// Gets the count of graphics commands.
    std::size_t getCount() const noexcept override;

    /// Draws the specified non-indexed primitives.
    ///
    /// @param vertexCount Number of vertices to draw.
    /// @param startVertexLocation Index of the first vertex to draw.
    void draw(
        std::size_t vertexCount,
        std::size_t startVertexLocation) override;

    /// Draws the specified indexed primitives.
    void drawIndexed(
        const std::shared_ptr<IndexBuffer>& indexBuffer,
        std::size_t indexCount,
        std::size_t startIndexLocation) override;

    /// Draws the specified instanced primitives.
    void drawInstanced(
        std::size_t vertexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startVertexLocation,
        std::size_t startInstanceLocation) override;

    /// Draws the specified indexed, instanced primitives.
    void drawIndexedInstanced(
        const std::shared_ptr<IndexBuffer>& indexBuffer,
        std::size_t indexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startIndexLocation,
        std::size_t startInstanceLocation) override;

    /// Sets a group of render targets.
    void setRenderPass(RenderPass&& renderPass) override;

    /// Sets the viewport dynamically to the rasterizer stage.
    void setViewport(const Viewport& viewport) override;

    /// Sets the scissor rectangle dynamically for a scissor test.
    void setScissorRect(const Rect2D& scissorRect) override;

    /// Sets the constant blend color and alpha values.
    void setBlendFactor(const Vector4& blendFactor) override;

    /// Sets a vertex buffer.
    void setVertexBuffer(int index, const std::shared_ptr<VertexBuffer>& vertexBuffer) override;

    /// Sets a vertex buffer.
    void setVertexBuffer(
        int index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        std::size_t offset) override;

    /// Sets a pipeline state.
    void setPipelineState(const std::shared_ptr<PipelineState>& pipelineState) override;

    /// Sets a constant buffer.
    void setConstantBuffer(int index, const std::shared_ptr<ConstantBuffer>& constantBuffer) override;

    /// Sets a constant buffer.
    void setConstantBuffer(
        int index,
        const std::shared_ptr<ConstantBuffer>& constantBuffer,
        std::size_t offset) override;

    /// Sets an empty texture to the specified slot.
    void setTexture(int index) override;

    /// Sets a texture to the specified slot.
    void setTexture(int index, const std::shared_ptr<gpu::Texture2D>& texture) override;

    /// Sets a texture to the specified slot.
    void setTexture(int index, const std::shared_ptr<RenderTarget2D>& texture) override;

    /// Sets a sampler state to the specified slot.
    void setSamplerState(int index, const std::shared_ptr<SamplerState>& samplerState) override;

private:
    VkCommandBuffer commandBuffer;
};

} // namespace pomdog::gpu::detail::vulkan
