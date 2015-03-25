// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_STENCILOPERATION_6EB1EB6D_6B2D_4B7B_BA58_1AEA11222D30_HPP
#define POMDOG_STENCILOPERATION_6EB1EB6D_6B2D_4B7B_BA58_1AEA11222D30_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class StencilOperation: std::uint8_t {
    Keep,

    Zero,

    Replace,

    IncrementSaturation,

    DecrementSaturation,

    Invert,

    Increment,

    Decrement,
};

}// namespace Pomdog

#endif // !defined(POMDOG_STENCILOPERATION_6EB1EB6D_6B2D_4B7B_BA58_1AEA11222D30_HPP)
