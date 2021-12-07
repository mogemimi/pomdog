// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog::Detail::Vulkan {

[[nodiscard]] VkFormat
ToSurfaceFormat(SurfaceFormat format) noexcept;

} // namespace Pomdog::Detail::Vulkan
