// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/render_target2d_vulkan.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail::vulkan {

RenderTarget2DVulkan::RenderTarget2DVulkan(
    VkDevice device,
    i32 pixelWidth,
    i32 pixelHeight,
    i32 levelCount,
    PixelFormat format,
    PixelFormat depthStencilFormat,
    i32 multiSampleCount)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

} // namespace pomdog::gpu::detail::vulkan
