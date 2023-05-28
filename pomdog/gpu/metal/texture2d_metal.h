// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/utility/errors.h"
#import <Metal/MTLTexture.h>

namespace pomdog::gpu::detail::metal {

class Texture2DMetal final : public Texture2D {
public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        id<MTLDevice> device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        PixelFormat format) noexcept;

    /// Gets the width of the texture data, in pixels.
    std::int32_t getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t getHeight() const noexcept override;

    /// Gets the mipmap level.
    std::int32_t getLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the texture.
    PixelFormat getFormat() const noexcept override;

    /// Sets texture data.
    void setData(const void* pixelData) override;

    /// Gets the pointer of the native texture resource.
    [[nodiscard]] id<MTLTexture>
    getTexture() const noexcept;

private:
    id<MTLTexture> texture_ = nullptr;
    std::int32_t pixelWidth_ = 0;
    std::int32_t pixelHeight_ = 0;
    std::int32_t levelCount_ = 0;
    PixelFormat format_ = PixelFormat::A8_UNorm;
};

} // namespace pomdog::gpu::detail::metal
