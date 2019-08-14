// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class DepthFormat : std::uint8_t {
    /// 16-bit buffer format. 16 bits for depth.
    Depth16,

    /// 32-bit buffer format. 24 bits for depth and 8 bits for stencil.
    ///
    /// The pixel format is only supported in certain devices for Metal, so you
    /// can use Depth32_Float_Stencil8_Uint instead.
    Depth24Stencil8,

    /// 32-bit floating-point buffer format. 32 bits for depth.
    Depth32,

    /// 32-bit floating-point buffer and 8 bit stencil buffer.
    Depth32_Float_Stencil8_Uint,

    /// Do not use depeh stencil buffer.
    None,
};

} // namespace Pomdog
