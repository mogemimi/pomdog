// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/ShaderPipelineStage.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
class Error;
class GraphicsDevice;
class Shader;
} // namespace Pomdog

namespace Pomdog::ShaderCompilers::HLSLCompiler {

/// Creates a shader from a string of source code.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
CreateShaderFromBinary(
    GraphicsDevice& graphicsDevice,
    const void* shaderSource,
    std::size_t byteLength,
    ShaderPipelineStage pipelineStage);

/// Creates a shader from precompiled shader binary data.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
CreateShaderFromSource(
    GraphicsDevice& graphicsDevice,
    const void* shaderSource,
    std::size_t byteLength,
    const std::string& entryPoint,
    ShaderPipelineStage pipelineStage,
    std::optional<std::string>&& currentDirectory);

} // namespace Pomdog::ShaderCompilers::HLSLCompiler
