// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog::Detail::Vulkan {

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

} // namespace Pomdog::Detail::Vulkan
