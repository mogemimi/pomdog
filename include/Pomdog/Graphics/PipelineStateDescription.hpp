// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PIPELINESTATEDESCRIPTION_E706BE44_HPP
#define POMDOG_PIPELINESTATEDESCRIPTION_E706BE44_HPP

#include "detail/ForwardDeclarations.hpp"
#include "BlendDescription.hpp"
#include "DepthStencilDescription.hpp"
#include "InputLayoutDescription.hpp"
#include "RasterizerDescription.hpp"
#include <memory>
#include <vector>

namespace Pomdog {

struct PipelineStateDescription {
    std::shared_ptr<Shader> VertexShader;
    std::shared_ptr<Shader> PixelShader;
    InputLayoutDescription InputLayout;
    BlendDescription BlendState;
    RasterizerDescription RasterizerState;
    DepthStencilDescription DepthStencilState;
};

} // namespace Pomdog

#endif // POMDOG_PIPELINESTATEDESCRIPTION_E706BE44_HPP
