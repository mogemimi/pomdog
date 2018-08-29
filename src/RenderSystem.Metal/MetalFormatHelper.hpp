// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#import <Metal/MTLTexture.h>

namespace Pomdog {
namespace Detail {
namespace Metal {

MTLPixelFormat ToPixelFormat(SurfaceFormat format) noexcept;
MTLPixelFormat ToPixelFormat(DepthFormat format) noexcept;

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
