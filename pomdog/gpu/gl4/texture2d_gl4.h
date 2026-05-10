// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/tagged.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {

/// Texture2DObjectGL4 is a type-safe wrapper around a GL texture object name.
using Texture2DObjectGL4 = pomdog::detail::Tagged<GLuint, Texture2D>;

/// Texture2DGL4 is the OpenGL 4 implementation of Texture2D.
class Texture2DGL4 final : public Texture2D {
private:
    std::optional<Texture2DObjectGL4> textureObject_ = {};
    i32 pixelWidth_ = 0;
    i32 pixelHeight_ = 0;
    i32 levelCount_ = 0;
    PixelFormat format_ = PixelFormat::A8_UNorm;

public:
    ~Texture2DGL4() override;

    /// Creates a GL texture object with the given dimensions, mip levels, and pixel format.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
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

    /// Copies the pixel data from texture to memory.
    void getData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const;

    /// Generates a set of mipmaps.
    void generateMipmap();

    /// Gets the handle of the native texture resource.
    [[nodiscard]] Texture2DObjectGL4
    getTextureHandle() const noexcept;
};

} // namespace pomdog::gpu::detail::gl4
