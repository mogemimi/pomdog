// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/vulkan/texture2d_vulkan.h"
#include <vulkan/vulkan.h>

namespace pomdog::gpu::detail::vulkan {

class RenderTarget2DVulkan final : public RenderTarget2D {
public:
    RenderTarget2DVulkan(
        VkDevice device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        PixelFormat format,
        PixelFormat depthStencilFormat,
        std::int32_t multiSampleCount);

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const noexcept override;

    /// Gets the mipmap level.
    std::int32_t GetLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the render target.
    PixelFormat GetFormat() const noexcept override;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    PixelFormat GetDepthStencilFormat() const noexcept override;

    /// Gets the size of the texture resource.
    Rectangle GetBounds() const noexcept override;

    /// Copies the pixel data from texture to memory.
    void GetData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const override;

private:
};

} // namespace pomdog::gpu::detail::vulkan
