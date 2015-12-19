// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class EffectVariableType : std::uint8_t {
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

} // namespace Pomdog
