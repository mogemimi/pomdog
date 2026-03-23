// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/blend_desc.h"
#include "pomdog/gpu/depth_stencil_desc.h"
#include "pomdog/gpu/input_layout_desc.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/rasterizer_desc.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class Shader;
} // namespace pomdog::gpu

namespace pomdog::gpu {

struct POMDOG_EXPORT PipelineDesc final {
    std::shared_ptr<Shader> vertexShader;
    std::shared_ptr<Shader> pixelShader;
    InputLayoutDesc inputLayout;

    /// Specifies the geometric primitive type for the input assembler stage.
    PrimitiveTopology primitiveTopology;

    BlendDesc blendState;
    RasterizerDesc rasterizerState;
    DepthStencilDesc depthStencilState;
    std::vector<PixelFormat> renderTargetViewFormats;
    PixelFormat depthStencilViewFormat;
    std::uint32_t multiSampleMask;
};

} // namespace pomdog::gpu
