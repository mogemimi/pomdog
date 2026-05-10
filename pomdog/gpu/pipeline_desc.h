// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/blend_desc.h"
#include "pomdog/gpu/depth_stencil_desc.h"
#include "pomdog/gpu/input_layout_desc.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/rasterizer_desc.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class Shader;
} // namespace pomdog::gpu

namespace pomdog::gpu {

/// PipelineDesc describes the complete state needed to create a graphics pipeline object.
struct POMDOG_EXPORT PipelineDesc final {
    /// The compiled vertex shader program.
    std::shared_ptr<Shader> vertexShader = nullptr;

    /// The compiled pixel (fragment) shader program.
    std::shared_ptr<Shader> pixelShader = nullptr;

    /// Description of the vertex buffer input layout.
    InputLayoutDesc inputLayout = {};

    /// Specifies the geometric primitive type for the input assembler stage.
    PrimitiveTopology primitiveTopology = PrimitiveTopology::TriangleList;

    /// Blend state for the output merger stage.
    BlendDesc blendState = {};

    /// Rasterizer state controlling fill mode, culling, and depth bias.
    RasterizerDesc rasterizerState = {};

    /// Depth and stencil test state.
    DepthStencilDesc depthStencilState = {};

    /// Pixel formats for each color attachment; one entry per render target.
    std::vector<PixelFormat> renderTargetViewFormats = {};

    /// Pixel format of the depth-stencil attachment; use `PixelFormat::Invalid` if none.
    PixelFormat depthStencilViewFormat = PixelFormat::Invalid;

    /// Coverage mask for multisample rendering; use `~u32{0u}` to enable all samples.
    u32 multiSampleMask = std::numeric_limits<u32>::max();
};

} // namespace pomdog::gpu
