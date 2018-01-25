// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog {
namespace Detail {
namespace Vulkan {

class SamplerStateVulkan final : public NativeSamplerState {
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

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
