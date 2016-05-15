// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ShaderPipelineStage.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <cstddef>
#include <string>

namespace Pomdog {

class GraphicsDevice;
class Shader;

namespace ShaderCompilers {

struct POMDOG_EXPORT MetalCompiler final {
    static std::unique_ptr<Shader> CreateShaderFromSource(
        GraphicsDevice & graphicsDevice,
        const void* shaderSource,
        std::size_t byteLength,
        const std::string& entryPoint,
        ShaderPipelineStage pipelineStage);
};

} // namespace ShaderCompilers
} // namespace Pomdog
