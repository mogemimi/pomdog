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
class Builder<Shader>::Impl {
public:
    std::reference_wrapper<Detail::AssetLoaderContext const> loaderContext;
    std::vector<std::uint8_t> shaderBlob;
    ShaderCompilers::ShaderPipelineStage pipelineStage;
    void const* shaderSource;
    std::size_t byteLength;
    std::string entryPoint;
    bool precompiled;

public:
    explicit Impl(Detail::AssetLoaderContext const& context);
};
//-----------------------------------------------------------------------
Builder<Shader>::Impl::Impl(Detail::AssetLoaderContext const& contextIn)
    : loaderContext(contextIn)
    , pipelineStage(ShaderPipelineStage::VertexShader)
    , shaderSource(nullptr)
    , byteLength(0)
    , precompiled(false)
{
}
//-----------------------------------------------------------------------
Builder<Shader>::Builder(Detail::AssetLoaderContext const& contextIn)
    : impl(std::make_shared<Impl>(contextIn))
{}
//-----------------------------------------------------------------------
Builder<Shader>::~Builder() = default;
//-----------------------------------------------------------------------
Builder<Shader> & Builder<Shader>::SetPipelineStage(
    ShaderCompilers::ShaderPipelineStage pipelineStageIn)
{
    POMDOG_ASSERT(impl);
    impl->pipelineStage = pipelineStageIn;
    return *this;
}
//-----------------------------------------------------------------------
Builder<Shader> & Builder<Shader>::SetGLSL(
    void const* shaderSourceIn, std::size_t byteLengthIn)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(shaderSourceIn != nullptr);
    POMDOG_ASSERT(byteLengthIn > 0);

    auto graphicsDevice = impl->loaderContext.get().GraphicsDevice.lock();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::GLSL)
    {
        impl->shaderSource = shaderSourceIn;
        impl->byteLength = byteLengthIn;
        impl->precompiled = false;
    }
    return *this;
}
//-----------------------------------------------------------------------
Builder<Shader> & Builder<Shader>::SetGLSLFromFile(std::string const& filePath)
{
    POMDOG_ASSERT(!filePath.empty());

    auto graphicsDevice = impl->loaderContext.get().GraphicsDevice.lock();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::GLSL)
    {
        impl->shaderBlob = ReadBinaryFile(impl->loaderContext.get().OpenStream(filePath));
        if (impl->shaderBlob.empty()) {
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to open file.");
        }

        impl->shaderSource = impl->shaderBlob.data();
        impl->byteLength = impl->shaderBlob.size();
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

    auto graphicsDevice = impl->loaderContext.get().GraphicsDevice.lock();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL)
    {
        impl->shaderSource = shaderSourceIn;
        impl->byteLength = byteLengthIn;
        impl->entryPoint = entryPointIn;
        impl->precompiled = false;
    }
    return *this;
}
//-----------------------------------------------------------------------
Builder<Shader> & Builder<Shader>::SetHLSLPrecompiled(
    void const* shaderSourceIn, std::size_t byteLengthIn)
{
    POMDOG_ASSERT(shaderSourceIn != nullptr);
    POMDOG_ASSERT(byteLengthIn > 0);

    auto graphicsDevice = impl->loaderContext.get().GraphicsDevice.lock();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL)
    {
        impl->shaderSource = shaderSourceIn;
        impl->byteLength = byteLengthIn;
        impl->precompiled = true;
    }
    return *this;
}
//-----------------------------------------------------------------------
Builder<Shader> & Builder<Shader>::SetHLSLFromFile(
    std::string const& filePath, std::string const& entryPointIn)
{
    POMDOG_ASSERT(!filePath.empty());
    POMDOG_ASSERT(!entryPointIn.empty());

    auto graphicsDevice = impl->loaderContext.get().GraphicsDevice.lock();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL)
    {
        impl->shaderBlob = ReadBinaryFile(impl->loaderContext.get().OpenStream(filePath));
        if (impl->shaderBlob.empty()) {
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to open file.");
        }

        impl->shaderSource = impl->shaderBlob.data();
        impl->byteLength = impl->shaderBlob.size();
        impl->entryPoint = entryPointIn;
        impl->precompiled = false;
    }
    return *this;
}
//-----------------------------------------------------------------------
std::unique_ptr<Shader> Builder<Shader>::Build()
{
    auto graphicsDevice = impl->loaderContext.get().GraphicsDevice.lock();
    POMDOG_ASSERT(graphicsDevice);

    POMDOG_ASSERT(impl->shaderSource != nullptr);
    POMDOG_ASSERT(impl->byteLength > 0);

    const auto shaderLanguage = graphicsDevice->GetSupportedLanguage();

    if (shaderLanguage == ShaderLanguage::GLSL)
    {
        return GLSLCompiler::CreateShader(*graphicsDevice,
            impl->shaderSource, impl->byteLength, impl->pipelineStage);
    }
    else if (shaderLanguage == ShaderLanguage::HLSL)
    {
        if (impl->precompiled) {
            return HLSLCompiler::CreateShaderFromBinary(*graphicsDevice,
                impl->shaderSource, impl->byteLength, impl->pipelineStage);
        }

        POMDOG_ASSERT(!impl->entryPoint.empty());
        return HLSLCompiler::CreateShaderFromSource(*graphicsDevice,
            impl->shaderSource, impl->byteLength, impl->entryPoint, impl->pipelineStage);
    }

    // error: FUS RO DAH!
    POMDOG_THROW_EXCEPTION(std::domain_error,
        "This shading language is not supported");
}
//-----------------------------------------------------------------------
} // namespace AssetBuilders
} // namespace Pomdog
