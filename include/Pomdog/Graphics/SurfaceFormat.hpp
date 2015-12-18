// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class SurfaceFormat: std::uint8_t {
    ///@brief 8-bit alpha only.
    A8_UNorm,

    ///@brief 8-bit alpha only.
    R8_UNorm,

    ///@remarks UNorm means "unsigned normalized" integer.
    R8G8_UNorm,

    ///@remarks UNorm means "unsigned normalized" integer.
    R8G8B8A8_UNorm,

    R10G10B10A2_UNorm,

    R11G11B10_Float,

    ///@remarks UNorm means "unsigned normalized" integer.
    B8G8R8A8_UNorm,

    R32_Float,

    R32G32B32A32_Float,

    R16G16_Float,

    R16G16B16A16_Float,

    ///@remarks UNorm means "unsigned normalized" integer.
    BlockComp1_UNorm,

    ///@remarks UNorm means "unsigned normalized" integer.
    BlockComp2_UNorm,

    ///@remarks UNorm means "unsigned normalized" integer.
    BlockComp3_UNorm,
};

} // namespace Pomdog
