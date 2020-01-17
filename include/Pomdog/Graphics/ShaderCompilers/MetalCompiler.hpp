// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/ShaderPipelineStage.hpp"
#include <cstddef>
#include <memory>
#include <string>

namespace Pomdog {
class GraphicsDevice;
class Shader;
} // namespace Pomdog

namespace Pomdog::ShaderCompilers {

struct POMDOG_EXPORT MetalCompiler final {
    /// Creates a shader from a string of source code.
    [[nodiscard]] static std::unique_ptr<Shader>
    CreateShaderFromSource(
        GraphicsDevice& graphicsDevice,
        const void* shaderSource,
        std::size_t byteLength,
        const std::string& entryPoint,
        ShaderPipelineStage pipelineStage);

    /// Creates a shader from a default library.
    [[nodiscard]] static std::unique_ptr<Shader>
    CreateShaderFromDefaultLibrary(
        GraphicsDevice& graphicsDevice,
        const std::string& entryPoint,
        ShaderPipelineStage pipelineStage);

    /// Creates a shader from precompiled '*.metallib' library data.
    [[nodiscard]] static std::unique_ptr<Shader>
    CreateShaderFromBinary(
        GraphicsDevice& graphicsDevice,
        const void* shaderSource,
        std::size_t byteLength,
        const std::string& entryPoint,
        ShaderPipelineStage pipelineStage);
};

} // namespace Pomdog::ShaderCompilers
