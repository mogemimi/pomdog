// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/InputLayoutDescription.hpp"
#include "Pomdog/Graphics/RasterizerDescription.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace Pomdog {

///@todo this is draft
enum class PrimitiveTopologyType : std::uint8_t {
    Triangle,
    Line,
};

using ConstantBufferBindSlotCollection = std::unordered_map<std::string, int>;

struct PipelineStateDescription {
    ConstantBufferBindSlotCollection ConstantBufferBindSlots;
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
