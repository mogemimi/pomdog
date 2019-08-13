// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#import <Metal/MTLTexture.h>

namespace Pomdog::Detail::Metal {

MTLPixelFormat ToPixelFormat(SurfaceFormat format) noexcept;
MTLPixelFormat ToPixelFormat(DepthFormat format) noexcept;

} // namespace Pomdog::Detail::Metal
