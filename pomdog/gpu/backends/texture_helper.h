// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {
enum class PixelFormat : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::TextureHelper {

[[nodiscard]] i32
computeMipmapLevelCount(
    i32 pixelWidth,
    i32 pixelHeight);

[[nodiscard]] i32
computeTextureSizeInBytes(
    i32 pixelWidth,
    i32 pixelHeight,
    i32 levelCount,
    PixelFormat format);

} // namespace pomdog::gpu::detail::TextureHelper
