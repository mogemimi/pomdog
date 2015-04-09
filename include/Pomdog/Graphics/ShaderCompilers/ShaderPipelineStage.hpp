// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SHADERPIPELINESTAGE_DCDFCEA1_HPP
#define POMDOG_SHADERPIPELINESTAGE_DCDFCEA1_HPP

#include <cstdint>

namespace Pomdog {
namespace ShaderCompilers {

enum class ShaderPipelineStage: std::uint8_t {
    VertexShader,
    PixelShader,
    //GeometryShader,
    //ComputeShader,
};

} // namespace ShaderCompilers
} // namespace Pomdog

#endif // POMDOG_SHADERPIPELINESTAGE_DCDFCEA1_HPP
