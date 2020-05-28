// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#import <Metal/MTLTexture.h>
#import <Metal/MTLDepthStencil.h>

namespace Pomdog::Detail::Metal {

[[nodiscard]] MTLPixelFormat ToPixelFormat(SurfaceFormat format) noexcept;
[[nodiscard]] MTLPixelFormat ToPixelFormat(DepthFormat format) noexcept;

[[nodiscard]] MTLCompareFunction ToComparisonFunction(ComparisonFunction compareFunction) noexcept;

} // namespace Pomdog::Detail::Metal
