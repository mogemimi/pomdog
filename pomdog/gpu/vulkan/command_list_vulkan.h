// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <memory>
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {
class PipelineStateVulkan;
class SwapChainVulkan;
} // namespace pomdog::gpu::detail::vulkan

namespace pomdog::gpu::detail::vulkan {

class CommandListVulkan final : public CommandList {
private:
    static constexpr u32 kMaxConstantBufferSlots = 4;
    static constexpr u32 kMaxTextureSlots = 4;

    struct BoundBuffer final {
        VkBuffer buffer = nullptr;
        VkDeviceSize offset = 0;
        VkDeviceSize range = 0;
    };

    ::VkDevice device_ = nullptr;
    ::VkCommandPool commandPool_ = nullptr;
    ::VkCommandBuffer commandBuffer_ = nullptr;
    ::VkDescriptorPool descriptorPool_ = nullptr;
    std::shared_ptr<PipelineStateVulkan> pipelineState_;
    unsafe_ptr<SwapChainVulkan> swapChain_ = nullptr;
    std::array<std::optional<BoundBuffer>, kMaxConstantBufferSlots> boundConstantBuffers_;
    std::array<VkImageView, kMaxTextureSlots> boundTextures_ = {};
    std::array<VkSampler, kMaxTextureSlots> boundSamplers_ = {};
    std::vector<::VkRenderPass> mrtRenderPasses_;
    std::vector<::VkFramebuffer> mrtFramebuffers_;
    ::VkIndexType indexType_ = {};
    bool isRecording_ = false;
    bool isInRenderPass_ = false;
    bool descriptorsDirty_ = false;

    void flushDescriptorSets();
    void clearBoundResources();

public:
    CommandListVulkan() = delete;

    CommandListVulkan(
        ::VkDevice device,
        ::VkCommandPool commandPool);

    ~CommandListVulkan() override;

    /// Gets the underlying Vulkan command buffer.
    [[nodiscard]] ::VkCommandBuffer
    getCommandBuffer() const noexcept;

    /// Declares that recording to the command list is completed.
    void close() override;

    /// Clears the graphics commands.
    void reset() override;

    /// Draws the specified non-indexed primitives.
    void draw(
        u32 vertexCount,
        u32 startVertexLocation) override;

    /// Draws the specified indexed primitives.
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

    /// Sets the swap chain for render pass operations.
    void setSwapChain(unsafe_ptr<SwapChainVulkan> swapChain) noexcept;

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
};

} // namespace pomdog::gpu::detail::vulkan
