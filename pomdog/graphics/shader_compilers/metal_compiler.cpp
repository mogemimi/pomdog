// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/ShaderCompilers/MetalCompiler.hpp"
#include "../../Graphics.Backends/ShaderBytecode.hpp"
#include "../../Graphics.Backends/ShaderCompileOptions.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using Pomdog::Detail::ShaderBytecode;
using Pomdog::Detail::ShaderCompileOptions;

namespace Pomdog::ShaderCompilers::MetalCompiler {

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
    POMDOG_ASSERT(graphicsDevice.GetSupportedLanguage() == ShaderLanguage::Metal);

    ShaderBytecode shaderBytecode;
    shaderBytecode.Code = shaderSource;
    shaderBytecode.ByteLength = byteLength;

    ShaderCompileOptions compileOptions;
    compileOptions.EntryPoint = entryPoint;
    compileOptions.Profile.PipelineStage = pipelineStage;
    compileOptions.Precompiled = false;

    return graphicsDevice.CreateShader(std::move(shaderBytecode), std::move(compileOptions));
}

[[nodiscard]] std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
CreateShaderFromDefaultLibrary(
    GraphicsDevice& graphicsDevice,
    const std::string& entryPoint,
    ShaderPipelineStage pipelineStage)
{
    POMDOG_ASSERT(!entryPoint.empty());
    POMDOG_ASSERT(graphicsDevice.GetSupportedLanguage() == ShaderLanguage::Metal);

    ShaderBytecode shaderBytecode;
    shaderBytecode.Code = nullptr;
    shaderBytecode.ByteLength = 0;

    ShaderCompileOptions compileOptions;
    compileOptions.EntryPoint = entryPoint;
    compileOptions.Profile.PipelineStage = pipelineStage;
    compileOptions.Precompiled = false;

    return graphicsDevice.CreateShader(std::move(shaderBytecode), std::move(compileOptions));
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
    POMDOG_ASSERT(graphicsDevice.GetSupportedLanguage() == ShaderLanguage::Metal);

    ShaderBytecode shaderBytecode;
    shaderBytecode.Code = shaderSource;
    shaderBytecode.ByteLength = byteLength;

    ShaderCompileOptions compileOptions;
    compileOptions.EntryPoint = entryPoint;
    compileOptions.Profile.PipelineStage = pipelineStage;
    compileOptions.Precompiled = true;

    return graphicsDevice.CreateShader(std::move(shaderBytecode), std::move(compileOptions));
}

} // namespace Pomdog::ShaderCompilers::MetalCompiler
