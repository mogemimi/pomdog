// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/shader_reflections/effect_reflection.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {

class EffectReflectionVulkan final : public EffectReflection {
public:
    EffectReflectionVulkan();

    std::vector<EffectConstantDescription>
    getConstantBuffers() const noexcept override;
};

} // namespace pomdog::gpu::detail::vulkan
