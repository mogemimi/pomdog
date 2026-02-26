// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"

namespace pomdog::gpu {
enum class PixelFormat : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::vulkan {

[[nodiscard]] VkFormat
ToSurfaceFormat(PixelFormat format) noexcept;

} // namespace pomdog::gpu::detail::vulkan
