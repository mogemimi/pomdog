// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "../../RenderSystem/ShaderBytecode.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/ShaderCompilers/GLSLCompiler.hpp"
#include "Pomdog/Graphics/ShaderCompilers/HLSLCompiler.hpp"
#include "Pomdog/Graphics/ShaderCompilers/MetalCompiler.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Graphics/ShaderPipelineStage.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/FileSystem.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <fstream>
#include <optional>
#include <regex>
#include <set>
#include <utility>
#include <vector>

using Pomdog::Detail::BinaryReader;
using Pomdog::Detail::ShaderBytecode;
using Pomdog::ShaderCompilers::GLSLCompiler;
using Pomdog::ShaderCompilers::HLSLCompiler;
using Pomdog::ShaderCompilers::MetalCompiler;

namespace Pomdog {
namespace AssetBuilders {
namespace {

std::optional<std::string> IncludeGLSLFilesRecursive(
    const std::string& path, std::set<std::string> & includes)
{
    if (FileSystem::IsDirectory(path)) {
        Log::Warning("Pomdog", "error: " + path + "is directory, not text file.");
        return std::nullopt;
    }

    std::ifstream input(path);
    if (!input) {
        return std::nullopt;
    }
    std::istreambuf_iterator<char> start(input);
    std::istreambuf_iterator<char> end;
    std::string text(start, end);
    input.close();

    auto currentDirectory = PathHelper::GetDirectoryName(PathHelper::Normalize(path));

    auto lines = StringHelper::Split(text, '\n');
    text.clear();
    for (const auto& line : lines) {
        std::regex includeRegex(R"(\s*#\s*include\s+\"([\w\.\/\\]+)\")");
        std::smatch match;

        bool matched = std::regex_match(line, match, includeRegex);
        if (!matched || match.size() != 2) {
            text += line;
            text += '\n';
            continue;
        }

        auto includePath = PathHelper::Join(currentDirectory, match[1]);
        if (includes.find(includePath) == includes.end()) {
            includes.insert(includePath);
            auto result = IncludeGLSLFilesRecursive(includePath, includes);
            if (!result) {
                return std::nullopt;
            }
            text += *result;
        }
        text += '\n';
    }

    return text;
}

} // unnamed namespace

class Builder<Shader>::Impl {
public:
    std::reference_wrapper<Detail::AssetLoaderContext const> loaderContext;
    std::vector<std::uint8_t> shaderBlob;
    ShaderPipelineStage pipelineStage;
    ShaderBytecode shaderBytecode;
    std::string entryPoint;
    std::optional<std::string> shaderFilePath;
    bool precompiled;
    bool fromLibrary;

public:
    explicit Impl(const Detail::AssetLoaderContext& context);

    std::shared_ptr<GraphicsDevice> GetDevice()
    {
        return loaderContext.get().GraphicsDevice.lock();
    }
};

Builder<Shader>::Impl::Impl(const Detail::AssetLoaderContext& contextIn)
    : loaderContext(contextIn)
    , pipelineStage(ShaderPipelineStage::VertexShader)
    , precompiled(false)
    , fromLibrary(false)
{
}

Builder<Shader>::Builder(
    const Detail::AssetLoaderContext& contextIn,
    ShaderPipelineStage pipelineStageIn)
    : impl(std::make_shared<Impl>(contextIn))
{
    POMDOG_ASSERT(impl);
    impl->pipelineStage = pipelineStageIn;
}

Builder<Shader>::~Builder() = default;

Builder<Shader> & Builder<Shader>::SetGLSL(
    const void* shaderSourceIn, std::size_t byteLengthIn)
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
        impl->shaderFilePath = std::nullopt;
    }
    return *this;
}

Builder<Shader> & Builder<Shader>::SetGLSLFromFile(const std::string& assetName)
{
    POMDOG_ASSERT(!assetName.empty());

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::GLSL) {
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

        {
            // NOTE: Using the #include in GLSL support
            std::set<std::string> includes;
            auto includeResult = IncludeGLSLFilesRecursive(
                PathHelper::Join(impl->loaderContext.get().RootDirectory, assetName),
                includes);

            if (includeResult) {
                auto & shaderCode = *includeResult;
                impl->shaderBlob.clear();
                impl->shaderBlob.resize(shaderCode.size() + 1);
                impl->shaderBlob[shaderCode.size()] = 0;
                std::memcpy(impl->shaderBlob.data(), shaderCode.data(), shaderCode.size());
            }
        }

        impl->shaderBytecode.Code = impl->shaderBlob.data();
        impl->shaderBytecode.ByteLength = impl->shaderBlob.size();
        impl->shaderFilePath = assetName;
    }
    return *this;
}

Builder<Shader> & Builder<Shader>::SetHLSL(
    const void* shaderSourceIn,
    std::size_t byteLengthIn,
    const std::string& entryPointIn)
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
        impl->shaderFilePath = std::nullopt;
    }
    return *this;
}

Builder<Shader> & Builder<Shader>::SetHLSLPrecompiled(
    const void* shaderSourceIn, std::size_t byteLengthIn)
{
    POMDOG_ASSERT(shaderSourceIn != nullptr);
    POMDOG_ASSERT(byteLengthIn > 0);

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL) {
        impl->shaderBytecode.Code = shaderSourceIn;
        impl->shaderBytecode.ByteLength = byteLengthIn;
        impl->precompiled = true;
        impl->shaderFilePath = std::nullopt;
    }
    return *this;
}

Builder<Shader> & Builder<Shader>::SetHLSLFromFile(
    const std::string& assetName, const std::string& entryPointIn)
{
    POMDOG_ASSERT(!assetName.empty());
    POMDOG_ASSERT(!entryPointIn.empty());

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL) {
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
        impl->shaderFilePath = assetName;
    }
    return *this;
}

Builder<Shader> & Builder<Shader>::SetMetal(
    const void* shaderSourceIn,
    std::size_t byteLengthIn,
    const std::string& entryPointIn)
{
    POMDOG_ASSERT(shaderSourceIn != nullptr);
    POMDOG_ASSERT(byteLengthIn > 0);
    POMDOG_ASSERT(!entryPointIn.empty());

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::Metal) {
        impl->shaderBytecode.Code = shaderSourceIn;
        impl->shaderBytecode.ByteLength = byteLengthIn;
        impl->entryPoint = entryPointIn;
        impl->precompiled = false;
        impl->shaderFilePath = std::nullopt;
    }
    return *this;
}

Builder<Shader> & Builder<Shader>::SetMetalFromFile(
    const std::string& assetName, const std::string& entryPointIn)
{
    POMDOG_ASSERT(!assetName.empty());
    POMDOG_ASSERT(!entryPointIn.empty());

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::Metal) {
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
        impl->fromLibrary = false;
    }
    return *this;
}

Builder<Shader> & Builder<Shader>::SetMetalFromLibrary(
    const std::string& entryPointIn)
{
    POMDOG_ASSERT(!entryPointIn.empty());

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::Metal) {
        impl->shaderBytecode.Code = nullptr;
        impl->shaderBytecode.ByteLength = 0;
        impl->entryPoint = entryPointIn;
        impl->precompiled = false;
        impl->fromLibrary = true;
    }
    return *this;
}

std::shared_ptr<Shader> Builder<Shader>::Build()
{
    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    const auto shaderLanguage = graphicsDevice->GetSupportedLanguage();
    std::optional<std::string> currentDirectory;
    if (impl->shaderFilePath) {
        currentDirectory = PathHelper::Normalize(
            PathHelper::GetDirectoryName(*impl->shaderFilePath));
    }

    switch (shaderLanguage) {
    case ShaderLanguage::GLSL: {
        POMDOG_ASSERT(impl->shaderBytecode.Code != nullptr);
        POMDOG_ASSERT(impl->shaderBytecode.ByteLength > 0);
        return GLSLCompiler::CreateShader(
            *graphicsDevice,
            impl->shaderBytecode.Code,
            impl->shaderBytecode.ByteLength,
            impl->pipelineStage,
            std::move(currentDirectory));
    }
    case ShaderLanguage::HLSL: {
        POMDOG_ASSERT(impl->shaderBytecode.Code != nullptr);
        POMDOG_ASSERT(impl->shaderBytecode.ByteLength > 0);
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
            impl->pipelineStage,
            std::move(currentDirectory));
    }
    case ShaderLanguage::Metal: {
        POMDOG_ASSERT(!impl->entryPoint.empty());
        if (impl->fromLibrary) {
            return MetalCompiler::CreateShaderFromLibrary(
                *graphicsDevice,
                impl->entryPoint,
                impl->pipelineStage);
        }
        POMDOG_ASSERT(impl->shaderBytecode.Code != nullptr);
        POMDOG_ASSERT(impl->shaderBytecode.ByteLength > 0);
        return MetalCompiler::CreateShaderFromSource(
            *graphicsDevice,
            impl->shaderBytecode.Code,
            impl->shaderBytecode.ByteLength,
            impl->entryPoint,
            impl->pipelineStage);
    }
    }

    // error: FUS RO DAH!
    POMDOG_THROW_EXCEPTION(std::domain_error,
        "This shading language is not supported");
}

} // namespace AssetBuilders
} // namespace Pomdog
