// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {

class ShaderBytecode final {
public:
    const void* code = nullptr;
    std::size_t byteLength = 0;

    /// Optional pointer to a FlatBuffers-encoded ShaderReflect binary.
    /// When non-null, the GL4 backend uses this to resolve constant buffer
    /// and sampler slot indices instead of requiring external bind hints.
    const void* reflectionData = nullptr;
    std::size_t reflectionByteLength = 0;
};

} // namespace pomdog::gpu::detail
