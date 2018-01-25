// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Utility/Optional.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog {
namespace Detail {
namespace Vulkan {

struct VulkanFormatHelper final {
    static Optional<VkFormat> ToVkFormat(SurfaceFormat format) noexcept;
};

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
