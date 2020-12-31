// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class EffectVariableType : std::uint8_t {
    Bool,

    /// 32-bit signed integer.
    Int32,

    /// 32-bit unsigned integer.
    UInt32,

    /// 32-bit floating point value.
    Float,

    /// 64-bit floating point value.
    Double,

    String,

    Texture1D,

    Texture2D,

    Texture3D,

    TextureCube,

    Void,
};

} // namespace Pomdog
