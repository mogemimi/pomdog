// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/depth_stencil_buffer.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/surface_format.hpp"
#include "pomdog/utility/errors.hpp"
#import <Metal/MTLTexture.h>

namespace Pomdog::Detail::Metal {

class DepthStencilBufferMetal final : public DepthStencilBuffer {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        id<MTLDevice> device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        SurfaceFormat depthStencilFormat,
        std::int32_t multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const noexcept override;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    SurfaceFormat GetFormat() const noexcept override;

    /// Gets the size of the texture resource.
    Rectangle GetBounds() const noexcept override;

    /// Gets the pointer of the depth stencil texture.
    id<MTLTexture> GetTexture() const noexcept;

private:
    id<MTLTexture> depthStencilTexture = nullptr;
    std::int32_t pixelWidth = 0;
    std::int32_t pixelHeight = 0;
    SurfaceFormat depthStencilFormat = SurfaceFormat::Invalid;
    bool multiSampleEnabled = false;
};

} // namespace Pomdog::Detail::Metal
