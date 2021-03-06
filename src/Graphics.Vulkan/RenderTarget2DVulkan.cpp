// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "RenderTarget2DVulkan.hpp"
#include "../Utility/Exception.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog::Detail::Vulkan {

RenderTarget2DVulkan::RenderTarget2DVulkan(
    VkDevice device,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format,
    SurfaceFormat depthStencilFormat,
    std::int32_t multiSampleCount)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

} // namespace Pomdog::Detail::Vulkan
