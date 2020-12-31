// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class InputElementFormat : std::uint8_t {
    /// A single-component, 32-bit floating-point format.
    Float,

    /// A two-component, 64-bit floating-point format.
    Float2,

    /// A three-componet, 96-bit floating-point format.
    Float3,

    /// A four-component, 128-bit floating-point format
    Float4,

    /// A four-component, 128-bit integer format
    Int4,

    /// A four-component, 128-bit unsigned integer format
    //UInt4,

    /// A four-component, 32-bit unsigned integer format.
    Byte4,

    /// A two-component, 32-bit floating-point format.
    HalfFloat2,

    /// A four-component, 64-bit floating-point format.
    HalfFloat4,
};

} // namespace Pomdog
