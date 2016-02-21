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
struct SamplerDesc;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::vulkan {

class SamplerStateVulkan final : public SamplerState {
private:
    ::VkDevice device_ = nullptr;
    ::VkSampler sampler_ = nullptr;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        ::VkDevice device,
        const SamplerDesc& descriptor) noexcept;

    ~SamplerStateVulkan();

    [[nodiscard]] ::VkSampler
    getSamplerState() const noexcept;
};

} // namespace pomdog::gpu::detail::vulkan
