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

/// Converts a PixelFormat to the equivalent MTLPixelFormat.
[[nodiscard]] MTLPixelFormat
toMTLPixelFormat(PixelFormat format) noexcept;

/// Converts a ComparisonFunction to the equivalent MTLCompareFunction.
[[nodiscard]] MTLCompareFunction
toMTLCompareFunction(ComparisonFunction compareFunction) noexcept;

} // namespace pomdog::gpu::detail::metal
