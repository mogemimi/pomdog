// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu::detail {

/// GraphicsCapabilities reports the resource limits of the underlying graphics device.
struct GraphicsCapabilities final {
    /// Maximum number of constant buffer slots available to shaders.
    i32 ConstantBufferSlotCount = 0;

    /// Maximum number of texture sampler slots available to shaders.
    i32 SamplerSlotCount = 0;
};

} // namespace pomdog::gpu::detail
