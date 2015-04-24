// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/ShaderCompilers/HLSLCompiler.hpp"
#include "../../RenderSystem/NativeGraphicsDevice.hpp"
#include "../../RenderSystem/ShaderBytecode.hpp"
#include "../../RenderSystem/ShaderCompileOptions.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace ShaderCompilers {

using Detail::ShaderBytecode;
using Detail::ShaderCompileOptions;
using Detail::ShaderPipelineStage;

//-----------------------------------------------------------------------
std::unique_ptr<Shader> HLSLCompiler::CreateShaderFromBinary(
    GraphicsDevice & graphicsDevice,
    void const* shaderSource,
    std::size_t byteLength,
    Detail::ShaderPipelineStage pipelineStage)
{
    POMDOG_ASSERT(shaderSource != nullptr);
    POMDOG_ASSERT(byteLength > 0);
    POMDOG_ASSERT(graphicsDevice.GetSupportedLanguage() == ShaderLanguage::HLSL);

    auto nativeGraphicsDevice = graphicsDevice.NativeGraphicsDevice();

    ShaderBytecode shaderBytecode;
    shaderBytecode.Code = shaderSource;
    shaderBytecode.ByteLength = byteLength;

    ShaderCompileOptions compileOptions;
    compileOptions.Profile.PipelineStage = pipelineStage;
    compileOptions.Precompiled = true;

    return nativeGraphicsDevice->CreateShader(shaderBytecode, compileOptions);
}
//-----------------------------------------------------------------------
std::unique_ptr<Shader> HLSLCompiler::CreateShaderFromSource(
    GraphicsDevice & graphicsDevice,
    void const* shaderSource,
    std::size_t byteLength,
    std::string const& entryPoint,
    Detail::ShaderPipelineStage pipelineStage)
{
    POMDOG_ASSERT(shaderSource != nullptr);
    POMDOG_ASSERT(byteLength > 0);
    POMDOG_ASSERT(graphicsDevice.GetSupportedLanguage() == ShaderLanguage::HLSL);

    auto nativeGraphicsDevice = graphicsDevice.NativeGraphicsDevice();

    ShaderBytecode shaderBytecode;
    shaderBytecode.Code = shaderSource;
    shaderBytecode.ByteLength = byteLength;

    ShaderCompileOptions compileOptions;
    compileOptions.EntryPoint = entryPoint;
    compileOptions.Profile.PipelineStage = pipelineStage;
    compileOptions.Profile.ShaderModel.Major = 4;
    compileOptions.Profile.ShaderModel.Minor = 0;
    compileOptions.Precompiled = false;

    return nativeGraphicsDevice->CreateShader(shaderBytecode, compileOptions);
}
//-----------------------------------------------------------------------
} // namespace ShaderCompilers
} // namespace Pomdog
