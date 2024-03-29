// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/pipeline_state.h"
#include <vulkan/vulkan.h>

namespace pomdog::gpu::detail::vulkan {

class PipelineStateVulkan final : public PipelineState {
public:
    PipelineStateVulkan(
        VkDevice device,
        const PipelineDescriptor& descriptor);

    ~PipelineStateVulkan();

private:
    VkDevice device;
    VkPipeline pipeline;
    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
};

} // namespace pomdog::gpu::detail::vulkan
