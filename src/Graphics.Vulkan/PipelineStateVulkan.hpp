// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog::Detail::Vulkan {

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

} // namespace Pomdog::Detail::Vulkan
