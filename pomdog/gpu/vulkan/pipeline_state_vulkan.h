// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
struct PipelineDescriptor;
} // namespace pomdog::gpu

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
