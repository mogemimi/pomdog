// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/texture.h"
#include "pomdog/gpu/texture_usage.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Rect2D;
} // namespace pomdog

namespace pomdog::gpu {
enum class PixelFormat : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu {

/// Texture2D is a 2D texture resource for sampling in shaders.
class POMDOG_EXPORT Texture2D : public Texture {
public:
    Texture2D() noexcept;
    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    virtual ~Texture2D() override;

    /// Gets the mipmap level.
    [[nodiscard]] virtual i32
    getLevelCount() const noexcept = 0;

    /// Gets how the texture is used in the graphics pipeline.
    [[nodiscard]] TextureUsage
    getUsage() const noexcept override;

    /// Sets texture data.
    virtual void
    setData(const void* pixelData) = 0;

    /// Sets texture data for a sub-region.
    virtual void
    setData(
        i32 mipLevel,
        const Rect2D& region,
        const void* pixelData,
        u32 bytesPerRow) = 0;
};

} // namespace pomdog::gpu
