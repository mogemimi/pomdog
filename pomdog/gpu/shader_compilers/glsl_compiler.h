// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/shader_pipeline_stage.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::gpu {
class GraphicsDevice;
class Shader;
} // namespace pomdog::gpu

namespace pomdog::gpu::shader_compilers::GLSLCompiler {

/// Creates a shader from a string of source code.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
createShader(
    GraphicsDevice& graphicsDevice,
    std::span<const u8> shaderBytecode,
    ShaderPipelineStage pipelineStage,
    std::optional<std::string>&& currentDirectory,
    std::span<const u8> reflectionBlob);

} // namespace pomdog::gpu::shader_compilers::GLSLCompiler
