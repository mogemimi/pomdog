// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/MTLDepthStencil.h>
#import <Metal/MTLTexture.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
enum class PixelFormat : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::metal {

[[nodiscard]] MTLPixelFormat ToPixelFormat(PixelFormat format) noexcept;

[[nodiscard]] MTLCompareFunction ToComparisonFunction(ComparisonFunction compareFunction) noexcept;

} // namespace pomdog::gpu::detail::metal
