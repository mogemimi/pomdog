// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/texture_usage.h"

namespace pomdog::gpu {
enum class PixelFormat : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu {

class POMDOG_EXPORT Texture {
public:
    Texture() = default;
    Texture(const Texture&) = delete;
    Texture(Texture&&) = default;

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = default;

    virtual ~Texture();

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] virtual i32
    getWidth() const noexcept = 0;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] virtual i32
    getHeight() const noexcept = 0;

    /// Gets the format of the pixel data in the texture.
    [[nodiscard]] virtual PixelFormat
    getFormat() const noexcept = 0;

    /// Gets how the texture is used in the graphics pipeline.
    [[nodiscard]] virtual TextureUsage
    getUsage() const noexcept = 0;
};

} // namespace pomdog::gpu
