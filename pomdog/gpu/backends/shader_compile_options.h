// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/shader_pipeline_stage.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {

struct ShaderModel final {
    u8 major = 0;
    u8 minor = 0;
};

struct ShaderProfile final {
    ShaderModel shaderModel = {};
    ShaderPipelineStage pipelineStage = ShaderPipelineStage::VertexShader;
};

struct ShaderCompileOptions final {
    std::string entryPoint = {};
    ShaderProfile profile = {};
    std::span<const u8> reflectionBlob = {};
    bool precompiled = false;
};

} // namespace pomdog::gpu::detail
