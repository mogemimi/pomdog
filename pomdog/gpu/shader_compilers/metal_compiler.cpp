// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/shader_compilers/metal_compiler.h"
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

namespace pomdog::gpu::shader_compilers::MetalCompiler {

[[nodiscard]] std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
createShaderFromSource(
    GraphicsDevice& graphicsDevice,
    std::span<const u8> shaderBytecode,
    const std::string& entryPoint,
    ShaderPipelineStage pipelineStage)
{
    POMDOG_ASSERT(!shaderBytecode.empty());
    POMDOG_ASSERT(graphicsDevice.getBackendKind() == GraphicsBackend::Metal);

    ShaderCompileOptions compileOptions;
    compileOptions.entryPoint = entryPoint;
    compileOptions.profile.pipelineStage = pipelineStage;
    compileOptions.precompiled = false;

    return graphicsDevice.createShader(shaderBytecode, std::move(compileOptions));
}

[[nodiscard]] std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
createShaderFromDefaultLibrary(
    GraphicsDevice& graphicsDevice,
    const std::string& entryPoint,
    ShaderPipelineStage pipelineStage)
{
    POMDOG_ASSERT(!entryPoint.empty());
    POMDOG_ASSERT(graphicsDevice.getBackendKind() == GraphicsBackend::Metal);

    auto shaderBytecode = std::span<const u8>{};

    ShaderCompileOptions compileOptions;
    compileOptions.entryPoint = entryPoint;
    compileOptions.profile.pipelineStage = pipelineStage;
    compileOptions.precompiled = false;

    return graphicsDevice.createShader(shaderBytecode, std::move(compileOptions));
}

[[nodiscard]] std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
createShaderFromBinary(
    GraphicsDevice& graphicsDevice,
    std::span<const u8> shaderBytecode,
    const std::string& entryPoint,
    ShaderPipelineStage pipelineStage)
{
    POMDOG_ASSERT(!entryPoint.empty());
    POMDOG_ASSERT(graphicsDevice.getBackendKind() == GraphicsBackend::Metal);

    ShaderCompileOptions compileOptions;
    compileOptions.entryPoint = entryPoint;
    compileOptions.profile.pipelineStage = pipelineStage;
    compileOptions.precompiled = true;

    return graphicsDevice.createShader(shaderBytecode, std::move(compileOptions));
}

} // namespace pomdog::gpu::shader_compilers::MetalCompiler
