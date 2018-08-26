// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ShaderPipelineStage.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace Pomdog {
namespace Detail {

struct ShaderModel {
    std::uint8_t Major;
    std::uint8_t Minor;
};

struct ShaderProfile {
    Pomdog::Detail::ShaderModel ShaderModel;
    ShaderPipelineStage PipelineStage;
};

struct ShaderMacro {
    std::string Name;
    std::string Definition;
};

struct ShaderCompileOptions {
    std::string EntryPoint;
    std::string CurrentDirectory;
    std::vector<ShaderMacro> PreprocessorMacros;
    ShaderProfile Profile;
    bool Precompiled;
};

} // namespace Detail
} // namespace Pomdog
