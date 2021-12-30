// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/depth_stencil_buffer.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/utility/errors.h"
#import <Metal/MTLTexture.h>

namespace pomdog::gpu::detail::metal {

class DepthStencilBufferMetal final : public DepthStencilBuffer {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        id<MTLDevice> device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        PixelFormat depthStencilFormat,
        std::int32_t multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const noexcept override;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    PixelFormat GetFormat() const noexcept override;

    /// Gets the size of the texture resource.
    Rectangle GetBounds() const noexcept override;

    /// Gets the pointer of the depth stencil texture.
    id<MTLTexture> GetTexture() const noexcept;

private:
    id<MTLTexture> depthStencilTexture = nullptr;
    std::int32_t pixelWidth = 0;
    std::int32_t pixelHeight = 0;
    PixelFormat depthStencilFormat = PixelFormat::Invalid;
    bool multiSampleEnabled = false;
};

} // namespace pomdog::gpu::detail::metal
