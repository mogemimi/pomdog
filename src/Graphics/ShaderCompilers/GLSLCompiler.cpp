// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/ShaderCompilers/GLSLCompiler.hpp"
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

//-----------------------------------------------------------------------
std::unique_ptr<Shader> GLSLCompiler::CreateShader(
    GraphicsDevice & graphicsDevice,
    void const* shaderSource,
    std::size_t byteLength,
    ShaderPipelineStage pipelineStage)
{
    POMDOG_ASSERT(shaderSource != nullptr);
    POMDOG_ASSERT(byteLength > 0);
    POMDOG_ASSERT(graphicsDevice.GetSupportedLanguage() == ShaderLanguage::GLSL);

    auto nativeGraphicsDevice = graphicsDevice.NativeGraphicsDevice();

    ShaderBytecode shaderBytecode;
    shaderBytecode.Code = shaderSource;
    shaderBytecode.ByteLength = byteLength;

    ShaderCompileOptions compileOptions;
    compileOptions.EntryPoint = "main";
    compileOptions.Profile.PipelineStage = pipelineStage;
    compileOptions.Precompiled = false;

    return nativeGraphicsDevice->CreateShader(shaderBytecode, compileOptions);
}
//-----------------------------------------------------------------------
} // namespace ShaderCompilers
} // namespace Pomdog
