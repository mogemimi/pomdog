// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>
#include <optional>

namespace Pomdog::Detail::Vulkan {

struct VulkanFormatHelper final {
    static std::optional<VkFormat> ToVkFormat(SurfaceFormat format) noexcept;
};

} // namespace Pomdog::Detail::Vulkan
