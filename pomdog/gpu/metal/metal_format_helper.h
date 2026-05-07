// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/MTLDepthStencil.h>
#import <Metal/MTLTexture.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
enum class ComparisonFunction : u8;
enum class PixelFormat : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::metal {

[[nodiscard]] MTLPixelFormat
toMTLPixelFormat(PixelFormat format) noexcept;

[[nodiscard]] MTLCompareFunction
toMTLCompareFunction(ComparisonFunction compareFunction) noexcept;

} // namespace pomdog::gpu::detail::metal
