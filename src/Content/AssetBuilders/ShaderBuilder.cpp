// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "../../Utility/PathHelper.hpp"
#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "Pomdog/Graphics/ShaderCompilers/GLSLCompiler.hpp"
#include "Pomdog/Graphics/ShaderCompilers/HLSLCompiler.hpp"
#include "Pomdog/Graphics/ShaderCompilers/ShaderPipelineStage.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <fstream>
#include <vector>
#include <utility>

namespace Pomdog {
namespace AssetBuilders {
namespace {

using ShaderCompilers::GLSLCompiler;
using ShaderCompilers::HLSLCompiler;
using ShaderCompilers::ShaderPipelineStage;

static std::vector<std::uint8_t> ReadBinaryFile(std::ifstream && streamIn)
{
    std::ifstream stream = std::move(streamIn);

    if (!stream) {
        return {};
    }

    stream.seekg(0, stream.end);
    auto const length = static_cast<std::size_t>(stream.tellg());
    stream.seekg(0, stream.beg);

    std::vector<std::uint8_t> result(length + 1, 0);
    stream.read(reinterpret_cast<char*>(result.data()), result.size());
    return std::move(result);
}

} // unnamed namespace
//-----------------------------------------------------------------------
// explicit instantiations
template class Builder<Shader>;
//-----------------------------------------------------------------------
Builder<Shader>::Builder(Detail::AssetLoaderContext const& contextIn)
    : loaderContext(contextIn)
    , pipelineStage(ShaderPipelineStage::VertexShader)
    , shaderSource(nullptr)
    , byteLength(0)
    , precompiled(false)
{
}
//-----------------------------------------------------------------------
Builder<Shader> & Builder<Shader>::SetPipelineStage(
    ShaderCompilers::ShaderPipelineStage pipelineStageIn)
{
    pipelineStage = pipelineStageIn;
    return *this;
}
//-----------------------------------------------------------------------
Builder<Shader> & Builder<Shader>::SetGLSL(
    void const* shaderSourceIn, std::size_t byteLengthIn)
{
    POMDOG_ASSERT(shaderSourceIn != nullptr);
    POMDOG_ASSERT(byteLengthIn > 0);

    auto graphicsDevice = loaderContext.get().GraphicsDevice.lock();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::GLSL)
    {
        shaderSource = shaderSourceIn;
        byteLength = byteLengthIn;
        precompiled = false;
    }
    return *this;
}
//-----------------------------------------------------------------------
Builder<Shader> & Builder<Shader>::SetGLSLFromFile(std::string const& filePath)
{
    POMDOG_ASSERT(!filePath.empty());

    auto graphicsDevice = loaderContext.get().GraphicsDevice.lock();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::GLSL)
    {
        shaderBlob = ReadBinaryFile(loaderContext.get().OpenStream(filePath));
        if (shaderBlob.empty()) {
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to open file.");
        }

        shaderSource = shaderBlob.data();
        byteLength = shaderBlob.size();
    }
    return *this;
}
//-----------------------------------------------------------------------
Builder<Shader> & Builder<Shader>::SetHLSL(
    void const* shaderSourceIn, std::size_t byteLengthIn,
    std::string const& entryPointIn)
{
    POMDOG_ASSERT(shaderSourceIn != nullptr);
    POMDOG_ASSERT(byteLengthIn > 0);
    POMDOG_ASSERT(!entryPointIn.empty());

    auto graphicsDevice = loaderContext.get().GraphicsDevice.lock();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL)
    {
        shaderSource = shaderSourceIn;
        byteLength = byteLengthIn;
        entryPoint = entryPointIn;
        precompiled = false;
    }
    return *this;
}
//-----------------------------------------------------------------------
Builder<Shader> & Builder<Shader>::SetHLSLPrecompiled(
    void const* shaderSourceIn, std::size_t byteLengthIn)
{
    POMDOG_ASSERT(shaderSourceIn != nullptr);
    POMDOG_ASSERT(byteLengthIn > 0);

    auto graphicsDevice = loaderContext.get().GraphicsDevice.lock();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL)
    {
        shaderSource = shaderSourceIn;
        byteLength = byteLengthIn;
        precompiled = true;
    }
    return *this;
}
//-----------------------------------------------------------------------
Builder<Shader> & Builder<Shader>::SetHLSLFromFile(
    std::string const& filePath, std::string const& entryPointIn)
{
    POMDOG_ASSERT(!filePath.empty());
    POMDOG_ASSERT(!entryPointIn.empty());

    auto graphicsDevice = loaderContext.get().GraphicsDevice.lock();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL)
    {
        shaderBlob = ReadBinaryFile(loaderContext.get().OpenStream(filePath));
        if (shaderBlob.empty()) {
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to open file.");
        }

        shaderSource = shaderBlob.data();
        byteLength = shaderBlob.size();
        entryPoint = entryPointIn;
        precompiled = false;
    }
    return *this;
}
//-----------------------------------------------------------------------
std::unique_ptr<Shader> Builder<Shader>::Build()
{
    auto graphicsDevice = loaderContext.get().GraphicsDevice.lock();
    POMDOG_ASSERT(graphicsDevice);

    POMDOG_ASSERT(shaderSource != nullptr);
    POMDOG_ASSERT(byteLength > 0);

    const auto shaderLanguage = graphicsDevice->GetSupportedLanguage();

    if (shaderLanguage == ShaderLanguage::GLSL)
    {
        return GLSLCompiler::CreateShader(*graphicsDevice,
            shaderSource, byteLength, pipelineStage);
    }
    else if (shaderLanguage == ShaderLanguage::HLSL)
    {
        if (precompiled) {
            return HLSLCompiler::CreateShaderFromBinary(*graphicsDevice,
                shaderSource, byteLength, pipelineStage);
        }

        POMDOG_ASSERT(!entryPoint.empty());
        return HLSLCompiler::CreateShaderFromSource(*graphicsDevice,
            shaderSource, byteLength, entryPoint, pipelineStage);
    }

    // error: FUS RO DAH!
    POMDOG_THROW_EXCEPTION(std::domain_error,
        "This shading language is not supported");
}
//-----------------------------------------------------------------------
} // namespace AssetBuilders
} // namespace Pomdog
