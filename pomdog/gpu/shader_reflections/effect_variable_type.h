// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

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

} // namespace pomdog::gpu
