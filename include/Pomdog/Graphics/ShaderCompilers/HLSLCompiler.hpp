// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_HLSLCOMPILER_223CB751_HPP
#define POMDOG_HLSLCOMPILER_223CB751_HPP

#include "Pomdog/Graphics/ShaderPipelineStage.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <cstddef>
#include <string>

namespace Pomdog {

class GraphicsDevice;
class Shader;

namespace ShaderCompilers {

struct POMDOG_EXPORT HLSLCompiler final {
    static std::unique_ptr<Shader> CreateShaderFromBinary(
        GraphicsDevice & graphicsDevice,
        void const* shaderSource,
        std::size_t byteLength,
        ShaderPipelineStage pipelineStage);

    static std::unique_ptr<Shader> CreateShaderFromSource(
        GraphicsDevice & graphicsDevice,
        void const* shaderSource,
        std::size_t byteLength,
        std::string const& entryPoint,
        ShaderPipelineStage pipelineStage);
};

} // namespace ShaderCompilers
} // namespace Pomdog

#endif // POMDOG_HLSLCOMPILER_223CB751_HPP
