// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "EffectReflectionVulkan.hpp"
#include "../Utility/Exception.hpp"
#include "Pomdog/Utility/Assert.hpp"

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
