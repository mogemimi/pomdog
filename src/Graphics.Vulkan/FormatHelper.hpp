// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog::Detail::Vulkan {

VkCompareOp ToComparisonFunction(ComparisonFunction compareFunction) noexcept;

} // namespace Pomdog::Detail::Vulkan
