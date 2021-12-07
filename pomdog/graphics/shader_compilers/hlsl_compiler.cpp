// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/shader_compilers/hlsl_compiler.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/backends/shader_bytecode.hpp"
#include "pomdog/graphics/backends/shader_compile_options.hpp"
#include "pomdog/graphics/graphics_device.hpp"
#include "pomdog/graphics/shader.hpp"
#include "pomdog/graphics/shader_language.hpp"
#include "pomdog/utility/assert.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using Pomdog::Detail::ShaderBytecode;
using Pomdog::Detail::ShaderCompileOptions;

namespace Pomdog::ShaderCompilers::HLSLCompiler {

[[nodiscard]] std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
CreateShaderFromBinary(
    GraphicsDevice& graphicsDevice,
    const void* shaderSource,
    std::size_t byteLength,
    ShaderPipelineStage pipelineStage)
{
    POMDOG_ASSERT(shaderSource != nullptr);
    POMDOG_ASSERT(byteLength > 0);
    POMDOG_ASSERT(graphicsDevice.GetSupportedLanguage() == ShaderLanguage::HLSL);

    ShaderBytecode shaderBytecode;
    shaderBytecode.Code = shaderSource;
    shaderBytecode.ByteLength = byteLength;

    ShaderCompileOptions compileOptions;
    compileOptions.Profile.PipelineStage = pipelineStage;
    compileOptions.Precompiled = true;

    return graphicsDevice.CreateShader(std::move(shaderBytecode), std::move(compileOptions));
}

[[nodiscard]] std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
CreateShaderFromSource(
    GraphicsDevice& graphicsDevice,
    const void* shaderSource,
    std::size_t byteLength,
    const std::string& entryPoint,
    ShaderPipelineStage pipelineStage,
    std::optional<std::string>&& currentDirectory)
{
    POMDOG_ASSERT(shaderSource != nullptr);
    POMDOG_ASSERT(byteLength > 0);
    POMDOG_ASSERT(graphicsDevice.GetSupportedLanguage() == ShaderLanguage::HLSL);

    ShaderBytecode shaderBytecode;
    shaderBytecode.Code = shaderSource;
    shaderBytecode.ByteLength = byteLength;

    ShaderCompileOptions compileOptions;
    compileOptions.EntryPoint = entryPoint;
    compileOptions.Profile.PipelineStage = pipelineStage;
    compileOptions.Profile.ShaderModel.Major = 4;
    compileOptions.Profile.ShaderModel.Minor = 0;
    compileOptions.Precompiled = false;

    if (currentDirectory) {
        compileOptions.CurrentDirectory = std::move(*currentDirectory);
    }

    return graphicsDevice.CreateShader(std::move(shaderBytecode), std::move(compileOptions));
}

} // namespace Pomdog::ShaderCompilers::HLSLCompiler
