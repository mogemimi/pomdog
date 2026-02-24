// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/MTLTexture.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::metal {

class RenderTarget2DMetal final : public RenderTarget2D {
public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        id<MTLDevice> device,
        i32 pixelWidth,
        i32 pixelHeight,
        i32 levelCount,
        PixelFormat format,
        i32 multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] i32
    getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] i32
    getHeight() const noexcept override;

    /// Gets the mipmap level.
    [[nodiscard]] i32
    getLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the render target.
    [[nodiscard]] PixelFormat
    getFormat() const noexcept override;

    /// Gets the size of the texture resource.
    [[nodiscard]] Rectangle
    getBounds() const noexcept override;

    /// Copies the pixel data from texture to memory.
    void getData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const override;

    /// Gets the pointer of the native texture resource.
    [[nodiscard]] id<MTLTexture>
    getTexture() const noexcept;

private:
    id<MTLTexture> texture_ = nullptr;
    i32 pixelWidth_ = 0;
    i32 pixelHeight_ = 0;
    i32 levelCount_ = 0;
    PixelFormat format_ = PixelFormat::A8_UNorm;
    bool multiSampleEnabled_ = false;
};

} // namespace pomdog::gpu::detail::metal
