// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ShaderPipelineStage.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace Pomdog {
namespace Detail {

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

} // namespace Detail
} // namespace Pomdog
