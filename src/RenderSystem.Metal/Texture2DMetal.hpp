// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#import <Metal/MTLTexture.h>

namespace Pomdog::Detail::Metal {

class Texture2DMetal final : public Texture2D {
public:
    Texture2DMetal(
        id<MTLDevice> device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format);

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const noexcept override;

    /// Gets the mipmap level.
    std::int32_t GetLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the texture.
    SurfaceFormat GetFormat() const noexcept override;

    /// Sets texture data.
    void SetData(const void* pixelData) override;

    /// Gets the pointer of the native texture resource.
    id<MTLTexture> GetTexture() const noexcept;

private:
    id<MTLTexture> texture = nullptr;
    std::int32_t pixelWidth = 0;
    std::int32_t pixelHeight = 0;
    std::int32_t levelCount = 0;
    SurfaceFormat format = SurfaceFormat::A8_UNorm;
};

} // namespace Pomdog::Detail::Metal
