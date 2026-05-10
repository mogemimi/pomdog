// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {

/// GraphicsCapabilities reports the resource limits of the underlying graphics device.
struct GraphicsCapabilities final {
    /// Maximum number of constant buffer slots available to shaders.
    std::size_t ConstantBufferSlotCount;

    /// Maximum number of texture sampler slots available to shaders.
    std::size_t SamplerSlotCount;
};

} // namespace pomdog::gpu::detail
