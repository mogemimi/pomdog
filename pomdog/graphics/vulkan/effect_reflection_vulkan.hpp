// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/effect_reflection.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog::Detail::Vulkan {

class EffectReflectionVulkan final : public EffectReflection {
public:
    EffectReflectionVulkan();

    std::vector<EffectConstantDescription>
    GetConstantBuffers() const noexcept override;
};

} // namespace Pomdog::Detail::Vulkan
