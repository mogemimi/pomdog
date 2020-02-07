// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog::Detail::Vulkan {

VkFormat ToSurfaceFormat(SurfaceFormat format) noexcept;

VkFormat ToDepthFormat(DepthFormat depthFormat) noexcept;

} // namespace Pomdog::Detail::Vulkan
