// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/pixel_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// A PresentationParameters describes the presentation parameters.
struct PresentationParameters final {
    /// The width of the swap chain, in pixels.
    std::int32_t BackBufferWidth = 0;

    /// The height of the swap chain, in pixels.
    std::int32_t BackBufferHeight = 0;

    /// The maximum rate at which the back buffers can be presented.
    std::int32_t PresentationInterval = 0;

    /// The multisample count for the back buffer.
    std::int32_t MultiSampleCount = 0;

    /// The format of the back buffer.
    PixelFormat BackBufferFormat = PixelFormat::Invalid;

    /// The depth stencil format for the back buffer.
    PixelFormat DepthStencilFormat = PixelFormat::Invalid;

    /// The value indicating the window is in full screen mode.
    bool IsFullScreen = false;
};

} // namespace pomdog
