// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/sampler_state.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
struct SamplerDescriptor;
} // namespace pomdog::gpu

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
