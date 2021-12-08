// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.hpp"
#include <vulkan/vulkan.h>

namespace pomdog::detail::vulkan {

[[nodiscard]] VkFormat
ToSurfaceFormat(SurfaceFormat format) noexcept;

} // namespace pomdog::detail::vulkan
