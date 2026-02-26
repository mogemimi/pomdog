// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"

namespace pomdog::gpu {
enum class ComparisonFunction : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::vulkan {

[[nodiscard]] VkCompareOp
ToComparisonFunction(ComparisonFunction compareFunction) noexcept;

} // namespace pomdog::gpu::detail::vulkan
