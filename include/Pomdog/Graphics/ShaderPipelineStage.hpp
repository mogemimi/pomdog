// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class ShaderPipelineStage : std::uint8_t {
    VertexShader,
    PixelShader,
    //GeometryShader,
    //ComputeShader,
};

} // namespace Pomdog
