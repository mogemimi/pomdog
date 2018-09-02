// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/ShaderPipelineStage.hpp"
#include <cstddef>
#include <memory>
#include <optional>
#include <string>

namespace Pomdog {

class GraphicsDevice;
class Shader;

namespace ShaderCompilers {

struct POMDOG_EXPORT GLSLCompiler final {
    static std::unique_ptr<Shader> CreateShader(
        GraphicsDevice& graphicsDevice,
        const void* shaderSource,
        std::size_t byteLength,
        ShaderPipelineStage pipelineStage,
        std::optional<std::string>&& currentDirectory);
};

} // namespace ShaderCompilers
} // namespace Pomdog
