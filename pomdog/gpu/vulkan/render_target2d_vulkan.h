// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/vulkan/texture2d_vulkan.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vulkan/vulkan.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {

class RenderTarget2DVulkan final : public RenderTarget2D {
public:
    RenderTarget2DVulkan(
        VkDevice device,
        i32 pixelWidth,
        i32 pixelHeight,
        i32 levelCount,
        PixelFormat format,
        PixelFormat depthStencilFormat,
        i32 multiSampleCount);

    /// Gets the width of the texture data, in pixels.
    i32
    getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    i32
    getHeight() const noexcept override;

    /// Gets the mipmap level.
    i32
    getLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the render target.
    PixelFormat
    getFormat() const noexcept override;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    PixelFormat
    getDepthStencilFormat() const noexcept override;

    /// Gets the size of the texture resource.
    Rectangle
    getBounds() const noexcept override;

    /// Copies the pixel data from texture to memory.
    void getData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const override;

private:
};

} // namespace pomdog::gpu::detail::vulkan
