// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/effect_reflection.h"
#include "pomdog/gpu/forward_declarations.h"
#include <vulkan/vulkan.h>

namespace pomdog::gpu::detail::vulkan {

class EffectReflectionVulkan final : public EffectReflection {
public:
    EffectReflectionVulkan();

    std::vector<EffectConstantDescription>
    GetConstantBuffers() const noexcept override;
};

} // namespace pomdog::gpu::detail::vulkan
