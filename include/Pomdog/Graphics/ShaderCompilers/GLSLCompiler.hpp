// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ShaderPipelineStage.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <memory>
#include <cstddef>

namespace Pomdog {

class GraphicsDevice;
class Shader;

namespace ShaderCompilers {

struct POMDOG_EXPORT GLSLCompiler final {
    static std::unique_ptr<Shader> CreateShader(
        GraphicsDevice & graphicsDevice,
        const void* shaderSource,
        std::size_t byteLength,
        ShaderPipelineStage pipelineStage,
        const Optional<std::string>& currentDirectory);
};

} // namespace ShaderCompilers
} // namespace Pomdog
