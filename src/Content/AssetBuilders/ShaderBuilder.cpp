// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "../../RenderSystem/ShaderBytecode.hpp"
#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Graphics/ShaderCompilers/GLSLCompiler.hpp"
#include "Pomdog/Graphics/ShaderCompilers/HLSLCompiler.hpp"
#include "Pomdog/Graphics/ShaderCompilers/MetalCompiler.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Graphics/ShaderPipelineStage.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <fstream>
#include <vector>
#include <utility>

using Pomdog::Detail::BinaryReader;
using Pomdog::Detail::ShaderBytecode;
using Pomdog::ShaderCompilers::GLSLCompiler;
using Pomdog::ShaderCompilers::HLSLCompiler;
using Pomdog::ShaderCompilers::MetalCompiler;

namespace Pomdog {
namespace AssetBuilders {

//-----------------------------------------------------------------------
// explicit instantiations
template class Builder<Shader>;
//-----------------------------------------------------------------------
class Builder<Shader>::Impl {
public:
    std::reference_wrapper<Detail::AssetLoaderContext const> loaderContext;
    std::vector<std::uint8_t> shaderBlob;
    ShaderPipelineStage pipelineStage;
    ShaderBytecode shaderBytecode;
    std::string entryPoint;
    bool precompiled;

public:
    explicit Impl(Detail::AssetLoaderContext const& context);

    std::shared_ptr<GraphicsDevice> GetDevice()
    {
        return loaderContext.get().GraphicsDevice.lock();
    }
};
//-----------------------------------------------------------------------
Builder<Shader>::Impl::Impl(Detail::AssetLoaderContext const& contextIn)
    : loaderContext(contextIn)
    , pipelineStage(ShaderPipelineStage::VertexShader)
    , precompiled(false)
{
}
//-----------------------------------------------------------------------
Builder<Shader>::Builder(
    Detail::AssetLoaderContext const& contextIn,
    ShaderPipelineStage pipelineStageIn)
    : impl(std::make_shared<Impl>(contextIn))
{
    POMDOG_ASSERT(impl);
    impl->pipelineStage = pipelineStageIn;
}
//-----------------------------------------------------------------------
Builder<Shader>::~Builder() = default;
//-----------------------------------------------------------------------
Builder<Shader> & Builder<Shader>::SetGLSL(
    void const* shaderSourceIn, std::size_t byteLengthIn)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(shaderSourceIn != nullptr);
    POMDOG_ASSERT(byteLengthIn > 0);

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::GLSL) {
        impl->shaderBytecode.Code = shaderSourceIn;
        impl->shaderBytecode.ByteLength = byteLengthIn;
        impl->precompiled = false;
    }
    return *this;
}
//-----------------------------------------------------------------------
Builder<Shader> & Builder<Shader>::SetGLSLFromFile(std::string const& assetName)
{
    POMDOG_ASSERT(!assetName.empty());

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::GLSL)
    {
        auto binaryFile = impl->loaderContext.get().OpenStream(assetName);

        if (!binaryFile.Stream) {
            POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to open file.");
        }

        if (binaryFile.SizeInBytes <= 0) {
            POMDOG_THROW_EXCEPTION(std::runtime_error, "The file is too small");
        }

        impl->shaderBlob = BinaryReader::ReadString<std::uint8_t>(
            binaryFile.Stream, binaryFile.SizeInBytes);

        if (impl->shaderBlob.empty()) {
            POMDOG_THROW_EXCEPTION(std::runtime_error, "The file is too small");
        }

        impl->shaderBytecode.Code = impl->shaderBlob.data();
        impl->shaderBytecode.ByteLength = impl->shaderBlob.size();
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

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL) {
        impl->shaderBytecode.Code = shaderSourceIn;
        impl->shaderBytecode.ByteLength = byteLengthIn;
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

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL) {
        impl->shaderBytecode.Code = shaderSourceIn;
        impl->shaderBytecode.ByteLength = byteLengthIn;
        impl->precompiled = true;
    }
    return *this;
}
//-----------------------------------------------------------------------
Builder<Shader> & Builder<Shader>::SetHLSLFromFile(
    std::string const& assetName, std::string const& entryPointIn)
{
    POMDOG_ASSERT(!assetName.empty());
    POMDOG_ASSERT(!entryPointIn.empty());

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL)
    {
        auto binaryFile = impl->loaderContext.get().OpenStream(assetName);

        if (!binaryFile.Stream) {
            POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to open file.");
        }

        if (binaryFile.SizeInBytes <= 0) {
            POMDOG_THROW_EXCEPTION(std::runtime_error, "The file is too small");
        }

        impl->shaderBlob = BinaryReader::ReadString<std::uint8_t>(
            binaryFile.Stream, binaryFile.SizeInBytes);

        if (impl->shaderBlob.empty()) {
            POMDOG_THROW_EXCEPTION(std::runtime_error, "The file is too small");
        }

        impl->shaderBytecode.Code = impl->shaderBlob.data();
        impl->shaderBytecode.ByteLength = impl->shaderBlob.size();
        impl->entryPoint = entryPointIn;
        impl->precompiled = false;
    }
    return *this;
}
//-----------------------------------------------------------------------
std::shared_ptr<Shader> Builder<Shader>::Build()
{
    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    POMDOG_ASSERT(impl->shaderBytecode.Code != nullptr);
    POMDOG_ASSERT(impl->shaderBytecode.ByteLength > 0);

    const auto shaderLanguage = graphicsDevice->GetSupportedLanguage();

    if (shaderLanguage == ShaderLanguage::GLSL) {
        return GLSLCompiler::CreateShader(
            *graphicsDevice,
            impl->shaderBytecode.Code,
            impl->shaderBytecode.ByteLength,
            impl->pipelineStage);
    }
    if (shaderLanguage == ShaderLanguage::HLSL) {
        if (impl->precompiled) {
            return HLSLCompiler::CreateShaderFromBinary(
                *graphicsDevice,
                impl->shaderBytecode.Code,
                impl->shaderBytecode.ByteLength,
                impl->pipelineStage);
        }
        POMDOG_ASSERT(!impl->entryPoint.empty());
        return HLSLCompiler::CreateShaderFromSource(
            *graphicsDevice,
            impl->shaderBytecode.Code,
            impl->shaderBytecode.ByteLength,
            impl->entryPoint,
            impl->pipelineStage);
    }
    if (shaderLanguage == ShaderLanguage::Metal) {
        POMDOG_ASSERT(!impl->entryPoint.empty());
        return MetalCompiler::CreateShaderFromSource(
            *graphicsDevice,
            impl->shaderBytecode.Code,
            impl->shaderBytecode.ByteLength,
            impl->entryPoint,
            impl->pipelineStage);
    }

    // error: FUS RO DAH!
    POMDOG_THROW_EXCEPTION(std::domain_error,
        "This shading language is not supported");
}
//-----------------------------------------------------------------------
} // namespace AssetBuilders
} // namespace Pomdog
