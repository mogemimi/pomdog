// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::TextureHelper {

[[nodiscard]] std::int32_t computeMipmapLevelCount(
    std::int32_t pixelWidth,
    std::int32_t pixelHeight);

[[nodiscard]] std::int32_t computeTextureSizeInBytes(
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    PixelFormat format);

} // namespace pomdog::gpu::detail::TextureHelper
