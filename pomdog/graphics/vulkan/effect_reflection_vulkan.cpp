// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/vulkan/effect_reflection_vulkan.hpp"
#include "pomdog/utility/assert.hpp"
#include "pomdog/utility/exception.hpp"

namespace Pomdog::Detail::Vulkan {
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

} // namespace Pomdog::Detail::Vulkan
