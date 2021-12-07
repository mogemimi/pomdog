// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.hpp"
#import <Metal/MTLDepthStencil.h>
#import <Metal/MTLTexture.h>

namespace Pomdog::Detail::Metal {

[[nodiscard]] MTLPixelFormat ToPixelFormat(SurfaceFormat format) noexcept;

[[nodiscard]] MTLCompareFunction ToComparisonFunction(ComparisonFunction compareFunction) noexcept;

} // namespace Pomdog::Detail::Metal
