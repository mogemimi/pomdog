// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/shader_pipeline_stage.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {

struct ShaderModel final {
    std::uint8_t major;
    std::uint8_t minor;
};

struct ShaderProfile final {
    ShaderModel shaderModel;
    ShaderPipelineStage pipelineStage;
};

struct ShaderMacro final {
    std::string name;
    std::string definition;
};

struct ShaderCompileOptions final {
    std::string entryPoint;
    std::string currentDirectory;
    std::vector<ShaderMacro> preprocessorMacros;
    ShaderProfile profile;
    bool precompiled;
};

} // namespace pomdog::gpu::detail
