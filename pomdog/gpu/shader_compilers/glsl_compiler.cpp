// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/shader_compilers/glsl_compiler.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/shader_compile_options.h"
#include "pomdog/gpu/graphics_backend.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::gpu::detail::ShaderCompileOptions;

namespace pomdog::gpu::shader_compilers::GLSLCompiler {

[[nodiscard]] std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
createShader(
    GraphicsDevice& graphicsDevice,
    std::span<const u8> shaderBytecode,
    ShaderPipelineStage pipelineStage,
    std::optional<std::string>&& currentDirectory,
    std::span<const u8> reflectionBlob)
{
    POMDOG_ASSERT(!shaderBytecode.empty());
    POMDOG_ASSERT(
        graphicsDevice.getBackendKind() == GraphicsBackend::OpenGL4 ||
        graphicsDevice.getBackendKind() == GraphicsBackend::WebGL);

    ShaderCompileOptions compileOptions;
    compileOptions.entryPoint = "main";
    compileOptions.profile.pipelineStage = pipelineStage;
    compileOptions.precompiled = false;
    compileOptions.reflectionBlob = reflectionBlob;

    if (currentDirectory) {
        compileOptions.currentDirectory = std::move(*currentDirectory);
    }

    return graphicsDevice.createShader(shaderBytecode, std::move(compileOptions));
}

} // namespace pomdog::gpu::shader_compilers::GLSLCompiler
