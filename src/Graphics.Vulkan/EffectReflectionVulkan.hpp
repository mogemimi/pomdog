// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/EffectReflection.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog::Detail::Vulkan {

class EffectReflectionVulkan final : public EffectReflection {
public:
    EffectReflectionVulkan();

    std::vector<EffectConstantDescription>
    GetConstantBuffers() const noexcept override;
};

} // namespace Pomdog::Detail::Vulkan
