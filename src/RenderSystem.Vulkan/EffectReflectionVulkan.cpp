// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include "EffectReflectionVulkan.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
namespace Detail {
namespace Vulkan {
namespace {

} // unnamed namespace

EffectReflectionVulkan::EffectReflectionVulkan()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

std::vector<EffectConstantDescription> EffectReflectionVulkan::GetConstantBuffers() const
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
