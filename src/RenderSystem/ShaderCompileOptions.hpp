// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SHADERCOMPILEOPTIONS_11A9FF1A_HPP
#define POMDOG_SHADERCOMPILEOPTIONS_11A9FF1A_HPP

#include "Pomdog/Graphics/ShaderCompilers/ShaderPipelineStage.hpp"
#include <cstdint>
#include <vector>
#include <string>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

using ShaderCompilers::ShaderPipelineStage;

class ShaderModel {
public:
    std::uint8_t Major;
    std::uint8_t Minor;
};

class ShaderProfile {
public:
    ShaderModel ShaderModel;
    ShaderPipelineStage PipelineStage;
};

class ShaderMacro {
public:
    std::string Name;
    std::string Definition;
};

class ShaderCompileOptions {
public:
    std::string EntryPoint;
    std::string CurrentDirectory;
    std::vector<ShaderMacro> PreprocessorMacros;
    ShaderProfile Profile;
    bool Precompiled;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_SHADERCOMPILEOPTIONS_11A9FF1A_HPP
