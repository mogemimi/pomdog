// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/vulkan/render_target2d_vulkan.hpp"
#include "pomdog/graphics/surface_format.hpp"
#include "pomdog/utility/assert.hpp"
#include "pomdog/utility/exception.hpp"

namespace pomdog::detail::vulkan {

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

} // namespace pomdog::detail::vulkan