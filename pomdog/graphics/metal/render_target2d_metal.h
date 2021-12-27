// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/render_target2d.hpp"
#include "pomdog/graphics/surface_format.hpp"
#include "pomdog/utility/errors.hpp"
#import <Metal/MTLTexture.h>

namespace pomdog::detail::metal {

class RenderTarget2DMetal final : public RenderTarget2D {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        id<MTLDevice> device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        std::int32_t multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const noexcept override;

    /// Gets the mipmap level.
    std::int32_t GetLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the render target.
    SurfaceFormat GetFormat() const noexcept override;

    /// Gets the size of the texture resource.
    Rectangle GetBounds() const noexcept override;

    /// Copies the pixel data from texture to memory.
    void GetData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const override;

    /// Gets the pointer of the native texture resource.
    id<MTLTexture> GetTexture() const noexcept;

private:
    id<MTLTexture> texture = nullptr;
    std::int32_t pixelWidth = 0;
    std::int32_t pixelHeight = 0;
    std::int32_t levelCount = 0;
    SurfaceFormat format = SurfaceFormat::A8_UNorm;
    bool multiSampleEnabled = false;
};

} // namespace pomdog::detail::metal
