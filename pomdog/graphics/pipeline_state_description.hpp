// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/blend_description.hpp"
#include "pomdog/graphics/depth_stencil_description.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/input_layout_description.hpp"
#include "pomdog/graphics/primitive_topology.hpp"
#include "pomdog/graphics/rasterizer_description.hpp"
#include "pomdog/graphics/surface_format.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

struct PipelineStateDescription final {
    // NOTE: Since OpenGL 4.1 and earlier, location attribute cannot be used,
    // so you can give hint by name.
    std::unordered_map<std::string, int> ConstantBufferBindHints;
    std::unordered_map<std::string, int> SamplerBindHints;

    std::shared_ptr<Shader> VertexShader;
    std::shared_ptr<Shader> PixelShader;
    InputLayoutDescription InputLayout;

    /// Specifies the geometric primitive type for the input assembler stage.
    Pomdog::PrimitiveTopology PrimitiveTopology;

    BlendDescription BlendState;
    RasterizerDescription RasterizerState;
    DepthStencilDescription DepthStencilState;
    std::vector<SurfaceFormat> RenderTargetViewFormats;
    SurfaceFormat DepthStencilViewFormat;
    std::uint32_t MultiSampleMask;
};

} // namespace Pomdog
