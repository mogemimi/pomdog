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
    initialize(
        id<MTLDevice> device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        PixelFormat depthStencilFormat,
        std::int32_t multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    std::int32_t getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t getHeight() const noexcept override;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    PixelFormat getFormat() const noexcept override;

    /// Gets the size of the texture resource.
    Rectangle getBounds() const noexcept override;

    /// Gets the pointer of the depth stencil texture.
    [[nodiscard]] id<MTLTexture>
    getTexture() const noexcept;

private:
    id<MTLTexture> depthStencilTexture_ = nullptr;
    std::int32_t pixelWidth_ = 0;
    std::int32_t pixelHeight_ = 0;
    PixelFormat depthStencilFormat_ = PixelFormat::Invalid;
    bool multiSampleEnabled_ = false;
};

} // namespace pomdog::gpu::detail::metal
