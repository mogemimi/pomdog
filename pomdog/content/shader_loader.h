// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/shader_pipeline_stage.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::gpu {
class GraphicsDevice;
class Shader;
} // namespace pomdog::gpu

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog {

/// Loads a shader file that matches the current graphics backend automatically.
/// Given a base file name (without extension), this function resolves the
/// platform-specific shader path based on the graphics device's supported
/// language:
///   - GLSL:  <directory>/glsl/<filePath>.glsl
///   - HLSL:  <directory>/d3d11/<filePath>.dxbc
///   - Metal: <directory>/metal/<filePath>.metal
///
/// This is designed for workflows where shaders are authored in Slang and
/// compiled offline to each target language, then bundled into an archive.
/// The caller only needs to specify the directory, the Slang file name
/// (without extension), and the entry point.
///
/// @param fs The file system context used to locate the file.
/// @param graphicsDevice The graphics device.
/// @param pipelineStage The shader pipeline stage (vertex or pixel).
/// @param directory The virtual directory containing the shader subdirectories (e.g. "/assets/shaders").
/// @param filePath The base shader file name without extension (e.g. "simple_effect_vs").
/// @param entryPoint The entry point function name (required for HLSL and Metal).
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<gpu::Shader>, std::unique_ptr<Error>>
loadShaderAutomagically(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    gpu::ShaderPipelineStage pipelineStage,
    const std::string& directory,
    const std::string& filePath,
    const std::string& entryPoint);

} // namespace pomdog
