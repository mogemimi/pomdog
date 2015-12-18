// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ShaderPipelineStage.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <cstddef>

namespace Pomdog {

class GraphicsDevice;
class Shader;

namespace ShaderCompilers {

struct POMDOG_EXPORT GLSLCompiler final {
    static std::unique_ptr<Shader> CreateShader(
        GraphicsDevice & graphicsDevice,
        void const* shaderSource,
        std::size_t byteLength,
        ShaderPipelineStage pipelineStage);
};

} // namespace ShaderCompilers
} // namespace Pomdog
