// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/input_classification.h"
#include "pomdog/gpu/input_element_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

/// InputElementDesc describes a single input element in the vertex layout.
struct InputElementDesc final {
    /// Byte offset from the beginning of the vertex.
    u32 byteOffset = 0;

    /// Data format of the input element.
    InputElementFormat format = InputElementFormat::Float32x1;
};

/// InputBufferLayoutDesc describes the layout of a single vertex buffer.
struct InputBufferLayoutDesc final {
    /// Descriptors for each input element.
    std::vector<InputElementDesc> elements = {};

    /// Size in bytes of per-vertex data.
    u32 strideBytes = 0;

    /// Index of the input slot.
    u16 inputSlot = 0;

    /// Number of instances to draw using the same per-instance data before
    /// advancing in the buffer by one element. Must be 0 for per-vertex data.
    ///
    /// Not all backends support arbitrary step rates. Vulkan requires the
    /// VK_EXT_vertex_attribute_divisor extension. Some mobile GPUs may not
    /// support values other than 0 or 1.
    u16 instanceStepRate = 0;

    /// Specifies whether the input data is per-vertex or per-instance.
    InputClassification inputSlotClass = InputClassification::PerVertex;
};

} // namespace pomdog::gpu
