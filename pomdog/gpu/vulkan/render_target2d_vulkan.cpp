// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/render_target2d_vulkan.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/exception.h"

namespace pomdog::gpu::detail::vulkan {

RenderTarget2DVulkan::RenderTarget2DVulkan(
    VkDevice device,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    PixelFormat format,
    PixelFormat depthStencilFormat,
    std::int32_t multiSampleCount)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

} // namespace pomdog::gpu::detail::vulkan
