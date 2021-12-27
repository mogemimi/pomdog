// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/effect_reflection.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include <vulkan/vulkan.h>

namespace pomdog::detail::vulkan {

class EffectReflectionVulkan final : public EffectReflection {
public:
    EffectReflectionVulkan();

    std::vector<EffectConstantDescription>
    GetConstantBuffers() const noexcept override;
};

} // namespace pomdog::detail::vulkan
