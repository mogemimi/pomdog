// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeEffectReflection.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog {
namespace Detail {
namespace Vulkan {

class EffectReflectionVulkan final : public NativeEffectReflection {
public:
    EffectReflectionVulkan();

    std::vector<EffectConstantDescription> GetConstantBuffers() const override;
};

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
