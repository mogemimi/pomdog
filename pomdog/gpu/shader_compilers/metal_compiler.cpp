// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/shader_compilers/metal_compiler.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/shader_bytecode.h"
#include "pomdog/gpu/backends/shader_compile_options.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_language.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::gpu::detail::ShaderBytecode;
using pomdog::gpu::detail::ShaderCompileOptions;

namespace pomdog::gpu::shader_compilers::MetalCompiler {

[[nodiscard]] std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
CreateShaderFromSource(
    GraphicsDevice& graphicsDevice,
    const void* shaderSource,
    std::size_t byteLength,
    const std::string& entryPoint,
    ShaderPipelineStage pipelineStage)
{
    POMDOG_ASSERT(shaderSource != nullptr);
    POMDOG_ASSERT(byteLength > 0);
    POMDOG_ASSERT(graphicsDevice.getSupportedLanguage() == ShaderLanguage::Metal);

    ShaderBytecode shaderBytecode;
    shaderBytecode.code = shaderSource;
    shaderBytecode.byteLength = byteLength;

    ShaderCompileOptions compileOptions;
    compileOptions.entryPoint = entryPoint;
    compileOptions.profile.pipelineStage = pipelineStage;
    compileOptions.precompiled = false;

    return graphicsDevice.createShader(std::move(shaderBytecode), std::move(compileOptions));
}

[[nodiscard]] std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
CreateShaderFromDefaultLibrary(
    GraphicsDevice& graphicsDevice,
    const std::string& entryPoint,
    ShaderPipelineStage pipelineStage)
{
    POMDOG_ASSERT(!entryPoint.empty());
    POMDOG_ASSERT(graphicsDevice.getSupportedLanguage() == ShaderLanguage::Metal);

    ShaderBytecode shaderBytecode;
    shaderBytecode.code = nullptr;
    shaderBytecode.byteLength = 0;

    ShaderCompileOptions compileOptions;
    compileOptions.entryPoint = entryPoint;
    compileOptions.profile.pipelineStage = pipelineStage;
    compileOptions.precompiled = false;

    return graphicsDevice.createShader(std::move(shaderBytecode), std::move(compileOptions));
}

[[nodiscard]] std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
CreateShaderFromBinary(
    GraphicsDevice& graphicsDevice,
    const void* shaderSource,
    std::size_t byteLength,
    const std::string& entryPoint,
    ShaderPipelineStage pipelineStage)
{
    POMDOG_ASSERT(!entryPoint.empty());
    POMDOG_ASSERT(graphicsDevice.getSupportedLanguage() == ShaderLanguage::Metal);

    ShaderBytecode shaderBytecode;
    shaderBytecode.code = shaderSource;
    shaderBytecode.byteLength = byteLength;

    ShaderCompileOptions compileOptions;
    compileOptions.entryPoint = entryPoint;
    compileOptions.profile.pipelineStage = pipelineStage;
    compileOptions.precompiled = true;

    return graphicsDevice.createShader(std::move(shaderBytecode), std::move(compileOptions));
}

} // namespace pomdog::gpu::shader_compilers::MetalCompiler
