// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Utility/Optional.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#import <Metal/MTLTexture.h>

namespace Pomdog {
namespace Detail {
namespace Metal {

struct MetalFormatHelper final {
    static Optional<MTLPixelFormat> ToMTLPixelFormat(SurfaceFormat format) noexcept;
};

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
