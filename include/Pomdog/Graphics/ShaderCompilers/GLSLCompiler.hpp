// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GLSLCOMPILER_9FED2927_HPP
#define POMDOG_GLSLCOMPILER_9FED2927_HPP

#include "ShaderPipelineStage.hpp"
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

#endif // POMDOG_GLSLCOMPILER_9FED2927_HPP
