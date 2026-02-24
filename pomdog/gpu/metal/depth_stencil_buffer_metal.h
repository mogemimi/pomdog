// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/depth_stencil_buffer.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/MTLTexture.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::metal {

class DepthStencilBufferMetal final : public DepthStencilBuffer {
private:
    id<MTLTexture> depthStencilTexture_ = nullptr;
    i32 pixelWidth_ = 0;
    i32 pixelHeight_ = 0;
    PixelFormat depthStencilFormat_ = PixelFormat::Invalid;
    bool multiSampleEnabled_ = false;

public:
    /// Initializes the depth stencil buffer.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        id<MTLDevice> device,
        i32 pixelWidth,
        i32 pixelHeight,
        PixelFormat depthStencilFormat,
        i32 multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] i32
    getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] i32
    getHeight() const noexcept override;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    [[nodiscard]] PixelFormat
    getFormat() const noexcept override;

    /// Gets the size of the texture resource.
    [[nodiscard]] Rectangle
    getBounds() const noexcept override;

    /// Gets the pointer of the depth stencil texture.
    [[nodiscard]] id<MTLTexture>
    getTexture() const noexcept;
};

} // namespace pomdog::gpu::detail::metal
