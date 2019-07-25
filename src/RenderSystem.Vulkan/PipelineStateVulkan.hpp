// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativePipelineState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog {
namespace Detail {
namespace Vulkan {

class PipelineStateVulkan final : public NativePipelineState {
public:
    PipelineStateVulkan(
        VkDevice device,
        const PipelineStateDescription& description);

private:
    VkPipeline pipeline;
};

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
