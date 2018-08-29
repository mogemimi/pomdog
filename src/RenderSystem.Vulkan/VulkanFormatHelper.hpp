// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>
#include <optional>

namespace Pomdog {
namespace Detail {
namespace Vulkan {

struct VulkanFormatHelper final {
    static std::optional<VkFormat> ToVkFormat(SurfaceFormat format) noexcept;
};

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
