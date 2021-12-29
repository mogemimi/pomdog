// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/effect_reflection_vulkan.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/exception.h"

namespace pomdog::gpu::detail::vulkan {
namespace {

} // namespace

EffectReflectionVulkan::EffectReflectionVulkan()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

std::vector<EffectConstantDescription>
EffectReflectionVulkan::GetConstantBuffers() const noexcept
{
    // FIXME: Not implemented yet.
    return {};
}

} // namespace pomdog::gpu::detail::vulkan
