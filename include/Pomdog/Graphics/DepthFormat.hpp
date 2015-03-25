// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_DEPTHFORMAT_5FF8041D_B10E_4EBC_A142_2BAAA12C234B_HPP
#define POMDOG_DEPTHFORMAT_5FF8041D_B10E_4EBC_A142_2BAAA12C234B_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class DepthFormat: std::uint8_t {
    ///@brief 16-bit buffer format. 16 bits for depth.
    Depth16,

    ///@brief 32-bit buffer format. 24 bits for depth and 8 bits for stencil.
    Depth24Stencil8,

    ///@brief 32-bit floating-point buffer format. 32 bits for depth.
    Depth32,

    ///@brief Do not use depeh stencil buffer.
    None,
};

}// namespace Pomdog

#endif // !defined(POMDOG_DEPTHFORMAT_5FF8041D_B10E_4EBC_A142_2BAAA12C234B_HPP)
