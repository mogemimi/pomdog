// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog::Detail::Vulkan {

[[nodiscard]] VkFormat
ToSurfaceFormat(SurfaceFormat format) noexcept;

} // namespace Pomdog::Detail::Vulkan
