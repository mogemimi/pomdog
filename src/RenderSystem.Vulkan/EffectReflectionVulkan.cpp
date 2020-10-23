// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "EffectReflectionVulkan.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog::Detail::Vulkan {
namespace {

} // namespace

EffectReflectionVulkan::EffectReflectionVulkan()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

std::vector<EffectConstantDescription> EffectReflectionVulkan::GetConstantBuffers() const
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

} // namespace Pomdog::Detail::Vulkan
