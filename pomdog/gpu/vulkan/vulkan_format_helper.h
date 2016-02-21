// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"

namespace pomdog::gpu {
enum class PixelFormat : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::vulkan {

[[nodiscard]] VkFormat
toSurfaceFormat(PixelFormat format) noexcept;

/// Chooses a supported depth/stencil VkFormat for the given PixelFormat.
/// Falls back (e.g. D24S8 → D32_SFLOAT_S8_UINT) when the GPU does not
/// support the default mapping.
[[nodiscard]] VkFormat
chooseDepthStencilFormat(VkPhysicalDevice physicalDevice, PixelFormat format) noexcept;

} // namespace pomdog::gpu::detail::vulkan
