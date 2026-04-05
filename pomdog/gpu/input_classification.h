// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// InputClassification specifies whether input data is per-vertex or per-instance.
enum class InputClassification : u8 {
    /// Input data is per-vertex data.
    PerVertex,

    /// Input data is per-instance data.
    PerInstance,
};

} // namespace pomdog::gpu
