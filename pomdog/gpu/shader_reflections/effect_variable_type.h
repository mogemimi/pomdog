// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

enum class EffectVariableType : u8 {
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

} // namespace pomdog::gpu
