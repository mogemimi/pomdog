// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

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
} // namespace Pomdog

namespace Pomdog::ShaderCompilers {

struct POMDOG_EXPORT GLSLCompiler final {
    /// Creates a shader from a string of source code.
    [[nodiscard]] static std::unique_ptr<Shader>
    CreateShader(
        GraphicsDevice& graphicsDevice,
        const void* shaderSource,
        std::size_t byteLength,
        ShaderPipelineStage pipelineStage,
        std::optional<std::string>&& currentDirectory);
};

} // namespace Pomdog::ShaderCompilers
