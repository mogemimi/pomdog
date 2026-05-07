// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/MTLTexture.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::metal {

class Texture2DMetal final : public Texture2D {
private:
    id<MTLTexture> texture_ = nullptr;
    i32 pixelWidth_ = 0;
    i32 pixelHeight_ = 0;
    i32 levelCount_ = 0;
    PixelFormat format_ = PixelFormat::A8_UNorm;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        id<MTLDevice> device,
        i32 pixelWidth,
        i32 pixelHeight,
        i32 levelCount,
        PixelFormat format) noexcept;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] i32
    getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] i32
    getHeight() const noexcept override;

    /// Gets the mipmap level.
    [[nodiscard]] i32
    getLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the texture.
    [[nodiscard]] PixelFormat
    getFormat() const noexcept override;

    /// Sets texture data.
    void setData(const void* pixelData) override;

    /// Sets texture data for a sub-region.
    void setData(
        i32 mipLevel,
        const Rect2D& region,
        const void* pixelData,
        u32 bytesPerRow) override;

    /// Gets the pointer of the native texture resource.
    [[nodiscard]] id<MTLTexture>
    getTexture() const noexcept;
};

} // namespace pomdog::gpu::detail::metal
