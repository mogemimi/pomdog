// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
struct PipelineDesc;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::vulkan {

class PipelineStateVulkan final : public PipelineState {
public:
    static constexpr u32 kDescriptorSetCount = 2;
    static constexpr u32 kMaxConstantBufferSlots = 4;
    static constexpr u32 kMaxTextureSlots = 4;

private:
    VkDevice device_ = nullptr;
    VkPipeline pipeline_ = nullptr;
    VkPipelineLayout pipelineLayout_ = nullptr;
    VkRenderPass renderPass_ = nullptr;
    std::array<VkDescriptorSetLayout, kDescriptorSetCount> descriptorSetLayouts_ = {};

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        const PipelineDesc& descriptor) noexcept;

    ~PipelineStateVulkan();

    /// Gets the Vulkan pipeline object.
    [[nodiscard]] VkPipeline
    getPipeline() const noexcept;

    /// Gets the Vulkan render pass object.
    [[nodiscard]] VkRenderPass
    getRenderPass() const noexcept;

    /// Gets the Vulkan pipeline layout.
    [[nodiscard]] VkPipelineLayout
    getPipelineLayout() const noexcept;

    /// Gets the descriptor set layouts.
    [[nodiscard]] const std::array<VkDescriptorSetLayout, kDescriptorSetCount>&
    getDescriptorSetLayouts() const noexcept;
};

} // namespace pomdog::gpu::detail::vulkan
