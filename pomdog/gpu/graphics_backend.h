// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// GraphicsBackend specifies the type of graphics backend.
enum class GraphicsBackend : u8 {
    /// Direct3D 11 backend.
    Direct3D11,

    /// OpenGL 4 backend.
    OpenGL4,

    /// Metal backend.
    Metal,

    /// WebGL backend.
    /// The implementation is the same as OpenGL 4, but some features
    /// may be limited to match WebGL's restrictions.
    WebGL,

    /// Vulkan backend.
    Vulkan,
};

} // namespace pomdog::gpu
