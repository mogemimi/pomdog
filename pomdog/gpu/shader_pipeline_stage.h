// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

enum class ShaderPipelineStage : u8 {
    VertexShader,
    PixelShader,
    //GeometryShader,
    //ComputeShader,
};

} // namespace pomdog::gpu

namespace pomdog {
using pomdog::gpu::ShaderPipelineStage;
} // namespace pomdog
