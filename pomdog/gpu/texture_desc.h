// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/texture_usage.h"

namespace pomdog::gpu {

/// Describes a texture resource to be created.
struct TextureDesc final {
    /// The width of the texture in pixels.
    i32 width = 0;

    /// The height of the texture in pixels.
    i32 height = 0;

    /// The number of mipmap levels. 1 means no mipmaps.
    i32 mipLevels = 1;

    /// The pixel format of the texture.
    PixelFormat format = PixelFormat::R8G8B8A8_UNorm;

    /// How the texture can be used in the pipeline.
    TextureUsage usage = TextureUsage::Sampled;
};

} // namespace pomdog::gpu
