// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include <cstdint>

namespace Pomdog {

/// A PresentationParameters describes the presentation parameters.
struct PresentationParameters final {
    /// The width of the swap chain, in pixels.
    std::int32_t BackBufferWidth;

    /// The height of the swap chain, in pixels.
    std::int32_t BackBufferHeight;

    /// The maximum rate at which the back buffers can be presented.
    std::int32_t PresentationInterval;

    /// The multisample count for the back buffer.
    std::int32_t MultiSampleCount;

    /// The format of the back buffer.
    SurfaceFormat BackBufferFormat;

    /// The depth stencil format for the back buffer.
    SurfaceFormat DepthStencilFormat = SurfaceFormat::Invalid;

    /// The value indicating the window is in full screen mode.
    bool IsFullScreen;
};

} // namespace Pomdog
