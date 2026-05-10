// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// ShaderPipelineStage specifies a programmable shader stage in the graphics pipeline.
enum class ShaderPipelineStage : u8 {
    /// The vertex shader stage; processes each vertex from the input assembler.
    VertexShader,

    /// The pixel (fragment) shader stage; computes the final color of each rasterized pixel.
    PixelShader,

    //GeometryShader,
    //ComputeShader,
};

} // namespace pomdog::gpu

namespace pomdog {
using pomdog::gpu::ShaderPipelineStage;
} // namespace pomdog
