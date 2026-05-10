// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {
enum class PixelFormat : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::TextureHelper {

/// Returns the number of mipmap levels needed to represent a texture of the
/// given dimensions, computed as `floor(log2(max(width, height))) + 1`.
[[nodiscard]] i32
computeMipmapLevelCount(
    i32 pixelWidth,
    i32 pixelHeight);

/// Returns the total byte size of a texture with the given dimensions, mip
/// level count, and pixel format, summing the storage of all mip levels.
/// For BCn compressed formats, row pitch and height are rounded up to the
/// nearest 4-texel block boundary.
[[nodiscard]] i32
computeTextureSizeInBytes(
    i32 pixelWidth,
    i32 pixelHeight,
    i32 levelCount,
    PixelFormat format);

} // namespace pomdog::gpu::detail::TextureHelper
