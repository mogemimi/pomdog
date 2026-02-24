// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/sampler_state.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vulkan/vulkan.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {

class SamplerStateVulkan final : public SamplerState {
public:
    SamplerStateVulkan(
        ::VkDevice device,
        const SamplerDescriptor& descriptor);

    ~SamplerStateVulkan();

    [[nodiscard]] ::VkSampler
    getSamplerState() const;

private:
    ::VkDevice device;
    ::VkSampler sampler;
};

} // namespace pomdog::gpu::detail::vulkan
