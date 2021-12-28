// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/blend_description.h"
#include "pomdog/gpu/depth_stencil_description.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/input_layout_description.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/rasterizer_description.h"
#include "pomdog/gpu/surface_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

struct PipelineStateDescription final {
    // NOTE: Since OpenGL 4.1 and earlier, location attribute cannot be used,
    // so you can give hint by name.
    std::unordered_map<std::string, int> ConstantBufferBindHints;
    std::unordered_map<std::string, int> SamplerBindHints;

    std::shared_ptr<Shader> VertexShader;
    std::shared_ptr<Shader> PixelShader;
    InputLayoutDescription InputLayout;

    /// Specifies the geometric primitive type for the input assembler stage.
    pomdog::PrimitiveTopology PrimitiveTopology;

    BlendDescription BlendState;
    RasterizerDescription RasterizerState;
    DepthStencilDescription DepthStencilState;
    std::vector<SurfaceFormat> RenderTargetViewFormats;
    SurfaceFormat DepthStencilViewFormat;
    std::uint32_t MultiSampleMask;
};

} // namespace pomdog
