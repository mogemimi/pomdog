// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/forward_declarations.h"

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
