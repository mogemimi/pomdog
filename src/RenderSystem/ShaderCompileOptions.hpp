// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SHADERCOMPILEOPTIONS_11A9FF1A_HPP
#define POMDOG_SHADERCOMPILEOPTIONS_11A9FF1A_HPP

#include "Pomdog/Graphics/ShaderPipelineStage.hpp"
#include <cstdint>
#include <vector>
#include <string>

namespace Pomdog {
namespace Detail {

struct ShaderModel {
    std::uint8_t Major;
    std::uint8_t Minor;
};

struct ShaderProfile {
    ShaderModel ShaderModel;
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

#endif // POMDOG_SHADERCOMPILEOPTIONS_11A9FF1A_HPP
