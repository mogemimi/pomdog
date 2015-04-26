// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PIPELINESTATEDESCRIPTION_E706BE44_HPP
#define POMDOG_PIPELINESTATEDESCRIPTION_E706BE44_HPP

#include "detail/ForwardDeclarations.hpp"
#include "BlendDescription.hpp"
#include "DepthFormat.hpp"
#include "DepthStencilDescription.hpp"
#include "InputLayoutDescription.hpp"
#include "RasterizerDescription.hpp"
#include "SurfaceFormat.hpp"
#include <memory>
#include <vector>

namespace Pomdog {

///@todo this is draft
enum class PrimitiveTopologyType : std::uint8_t {
    Triangle,
    Line,
};

struct PipelineStateDescription {
    std::shared_ptr<Shader> VertexShader;
    std::shared_ptr<Shader> PixelShader;
    InputLayoutDescription InputLayout;
    BlendDescription BlendState;
    RasterizerDescription RasterizerState;
    DepthStencilDescription DepthStencilState;
    std::vector<SurfaceFormat> RenderTargetViewFormats;
    DepthFormat DepthStencilViewFormat;
    std::uint32_t MultiSampleMask;
    PrimitiveTopologyType PrimitiveTopologyType;
};

} // namespace Pomdog

#endif // POMDOG_PIPELINESTATEDESCRIPTION_E706BE44_HPP
