// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/pipeline_state.hpp"
#include <vulkan/vulkan.h>

namespace pomdog::detail::vulkan {

class PipelineStateVulkan final : public PipelineState {
public:
    PipelineStateVulkan(
        VkDevice device,
        const PipelineStateDescription& description);

    ~PipelineStateVulkan();

private:
    VkDevice device;
    VkPipeline pipeline;
    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
};

} // namespace pomdog::detail::vulkan
