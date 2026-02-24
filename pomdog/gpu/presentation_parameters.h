// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/pixel_format.h"

namespace pomdog::gpu {

/// A PresentationParameters describes the presentation parameters.
struct PresentationParameters final {
    /// The width of the swap chain, in pixels.
    i32 backBufferWidth = 0;

    /// The height of the swap chain, in pixels.
    i32 backBufferHeight = 0;

    /// The maximum rate at which the back buffers can be presented.
    i32 presentationInterval = 0;

    /// The multisample count for the back buffer.
    i32 multiSampleCount = 0;

    /// The format of the back buffer.
    PixelFormat backBufferFormat = PixelFormat::Invalid;

    /// The depth stencil format for the back buffer.
    PixelFormat depthStencilFormat = PixelFormat::Invalid;

    /// The value indicating the window is in full screen mode.
    bool isFullScreen = false;
};

} // namespace pomdog::gpu
