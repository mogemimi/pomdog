// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/input_element.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

/// InputLayoutDesc describes the full input layout for a pipeline.
struct InputLayoutDesc final {
    /// Layout descriptions for multiple vertex buffers.
    std::vector<InputBufferLayoutDesc> vertexBuffers = {};
};

} // namespace pomdog::gpu
