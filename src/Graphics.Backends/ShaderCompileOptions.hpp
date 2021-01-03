// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Graphics/ShaderPipelineStage.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {

struct ShaderModel final {
    std::uint8_t Major;
    std::uint8_t Minor;
};

struct ShaderProfile final {
    Pomdog::Detail::ShaderModel ShaderModel;
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

} // namespace Pomdog::Detail
