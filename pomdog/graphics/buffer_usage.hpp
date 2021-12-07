// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

/// Identifies the expected usage pattern of graphics buffers during rendering.
enum class BufferUsage : std::uint8_t {
    /// A resource that can only be read by the GPU. It cannot be accessed by the CPU.
    ///
    /// A immutable resource must be initialized when it is created, since it cannot be changed after creation.
    Immutable,

    /// A resource that will be modified repeatedly. It is accessible by both the GPU (read only) and the CPU (write only).
    ///
    /// A dynamic resource is a good choice for a resource that will be updated by the CPU per frame.
    Dynamic,
};

} // namespace Pomdog
