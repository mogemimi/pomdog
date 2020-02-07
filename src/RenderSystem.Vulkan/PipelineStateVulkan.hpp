// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativePipelineState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog::Detail::Vulkan {

class PipelineStateVulkan final : public NativePipelineState {
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
