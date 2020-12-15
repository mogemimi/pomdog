// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/InputLayoutDescription.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/RasterizerDescription.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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
