// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.h"
#include "pomdog/graphics/sampler_state.h"
#include <vulkan/vulkan.h>

namespace pomdog::detail::vulkan {

class SamplerStateVulkan final : public SamplerState {
public:
    SamplerStateVulkan(
        ::VkDevice device,
        const SamplerDescription& description);

    ~SamplerStateVulkan();

    ::VkSampler GetSamplerState() const;

private:
    ::VkDevice device;
    ::VkSampler sampler;
};

} // namespace pomdog::detail::vulkan
