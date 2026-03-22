// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/blend_descriptor.h"
#include "pomdog/gpu/depth_stencil_descriptor.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/input_layout_descriptor.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/rasterizer_descriptor.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

struct PipelineDescriptor final {
    std::shared_ptr<Shader> vertexShader;
    std::shared_ptr<Shader> pixelShader;
    InputLayoutDescriptor inputLayout;

    /// Specifies the geometric primitive type for the input assembler stage.
    PrimitiveTopology primitiveTopology;

    BlendDescriptor blendState;
    RasterizerDescriptor rasterizerState;
    DepthStencilDescriptor depthStencilState;
    std::vector<PixelFormat> renderTargetViewFormats;
    PixelFormat depthStencilViewFormat;
    std::uint32_t multiSampleMask;
};

} // namespace pomdog::gpu
