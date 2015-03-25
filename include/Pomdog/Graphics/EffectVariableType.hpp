// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTVARIABLETYPE_54C9D81C_73AF_47EF_828C_CAF40803402F_HPP
#define POMDOG_EFFECTVARIABLETYPE_54C9D81C_73AF_47EF_828C_CAF40803402F_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class EffectVariableType: std::uint8_t {
    Bool,

    ///@brief 32-bit signed integer.
    Int32,

    ///@brief 32-bit unsigned integer.
    UInt32,

    ///@brief 32-bit floating point value.
    Float,

    ///@brief 64-bit floating point value.
    Double,

    String,

    Texture1D,

    Texture2D,

    Texture3D,

    TextureCube,

    Void,
};

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTVARIABLETYPE_54C9D81C_73AF_47EF_828C_CAF40803402F_HPP)
