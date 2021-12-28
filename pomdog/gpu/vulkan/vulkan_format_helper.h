// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/forward_declarations.h"
#include <vulkan/vulkan.h>

namespace pomdog::detail::vulkan {

[[nodiscard]] VkFormat
ToSurfaceFormat(SurfaceFormat format) noexcept;

} // namespace pomdog::detail::vulkan
