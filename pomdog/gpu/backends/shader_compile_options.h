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
    std::uint8_t Major;
    std::uint8_t Minor;
};

struct ShaderProfile final {
    pomdog::gpu::detail::ShaderModel ShaderModel;
    ShaderPipelineStage PipelineStage;
};

struct ShaderMacro final {
    std::string Name;
    std::string Definition;
};

struct ShaderCompileOptions final {
    std::string EntryPoint;
    std::string CurrentDirectory;
    std::vector<ShaderMacro> PreprocessorMacros;
    ShaderProfile Profile;
    bool Precompiled;
};

} // namespace pomdog::gpu::detail