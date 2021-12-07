// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/graphics_command_list.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog::Detail::Vulkan {

class GraphicsCommandListVulkan final : public GraphicsCommandList {
public:
    GraphicsCommandListVulkan();

    /// Declares that recording to the command list is completed.
    void Close() override;

    /// Clears the graphics commands.
    void Reset() override;

    /// Gets the count of graphics commands.
    std::size_t GetCount() const noexcept override;

    /// Draws the specified non-indexed primitives.
    ///
    /// @param vertexCount Number of vertices to draw.
    /// @param startVertexLocation Index of the first vertex to draw.
    void Draw(
        std::size_t vertexCount,
        std::size_t startVertexLocation) override;

    /// Draws the specified indexed primitives.
    void DrawIndexed(
        const std::shared_ptr<IndexBuffer>& indexBuffer,
        std::size_t indexCount,
        std::size_t startIndexLocation) override;

    /// Draws the specified instanced primitives.
    void DrawInstanced(
        std::size_t vertexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startVertexLocation,
        std::size_t startInstanceLocation) override;

    /// Draws the specified indexed, instanced primitives.
    void DrawIndexedInstanced(
        const std::shared_ptr<IndexBuffer>& indexBuffer,
        std::size_t indexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startIndexLocation,
        std::size_t startInstanceLocation) override;

    /// Sets a group of render targets.
    void SetRenderPass(RenderPass&& renderPass) override;

    /// Sets the viewport dynamically to the rasterizer stage.
    void SetViewport(const Viewport& viewport) override;

    /// Sets the scissor rectangle dynamically for a scissor test.
    void SetScissorRect(const Rectangle& scissorRect) override;

    /// Sets the constant blend color and alpha values.
    void SetBlendFactor(const Vector4& blendFactor) override;

    /// Sets a vertex buffer.
    void SetVertexBuffer(int index, const std::shared_ptr<VertexBuffer>& vertexBuffer) override;

    /// Sets a vertex buffer.
    void SetVertexBuffer(
        int index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        std::size_t offset) override;

    /// Sets a pipeline state.
    void SetPipelineState(const std::shared_ptr<PipelineState>& pipelineState) override;

    /// Sets a constant buffer.
    void SetConstantBuffer(int index, const std::shared_ptr<ConstantBuffer>& constantBuffer) override;

    /// Sets a constant buffer.
    void SetConstantBuffer(
        int index,
        const std::shared_ptr<ConstantBuffer>& constantBuffer,
        std::size_t offset) override;

    /// Sets an empty texture to the specified slot.
    void SetTexture(int index) override;

    /// Sets a texture to the specified slot.
    void SetTexture(int index, const std::shared_ptr<Texture2D>& texture) override;

    /// Sets a texture to the specified slot.
    void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture) override;

    /// Sets a sampler state to the specified slot.
    void SetSamplerState(int index, const std::shared_ptr<SamplerState>& samplerState) override;

private:
    VkCommandBuffer commandBuffer;
};

} // namespace Pomdog::Detail::Vulkan
