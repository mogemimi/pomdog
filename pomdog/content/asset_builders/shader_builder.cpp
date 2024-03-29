// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/asset_builders/shader_builder.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/gpu/backends/shader_bytecode.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_compilers/glsl_compiler.h"
#include "pomdog/gpu/shader_compilers/hlsl_compiler.h"
#include "pomdog/gpu/shader_compilers/metal_compiler.h"
#include "pomdog/gpu/shader_language.h"
#include "pomdog/gpu/shader_pipeline_stage.h"
#include "pomdog/logging/log.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/path_helper.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <fstream>
#include <optional>
#include <regex>
#include <set>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace BinaryReader = pomdog::detail::BinaryReader;
using pomdog::gpu::Shader;
using pomdog::gpu::ShaderLanguage;
using pomdog::gpu::detail::ShaderBytecode;

namespace pomdog::AssetBuilders {
namespace {

using namespace pomdog::gpu::shader_compilers;

[[nodiscard]] std::optional<std::string>
IncludeGLSLFilesRecursive(const std::string& path, std::set<std::string>& includes)
{
    if (FileSystem::isDirectory(path)) {
        Log::Warning("pomdog", "error: " + path + "is directory, not text file.");
        return std::nullopt;
    }

    std::ifstream input{path, std::ifstream::binary};
    if (!input) {
        return std::nullopt;
    }
    std::istreambuf_iterator<char> start(input);
    std::istreambuf_iterator<char> end;
    std::string text(start, end);
    input.close();

    auto currentDirectory = filepaths::getDirectoryName(filepaths::normalize(path));

    auto copiedText = text;
    text.clear();
    auto lines = strings::split(copiedText, '\n');

    for (const auto& lineView : lines) {
        std::string line{lineView};
        std::regex includeRegex(R"(\s*#\s*include\s+\"([\w\.\/\\]+)\")");
        std::smatch match;

        bool matched = std::regex_match(line, match, includeRegex);
        if (!matched || match.size() != 2) {
            text += line;
            text += '\n';
            continue;
        }

        auto includePath = filepaths::join(currentDirectory, match[1].str());
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

#if defined(__GNUC__) && !defined(__clang__)
    return text;
#else
    return std::move(text);
#endif
}

} // namespace

class Builder<Shader>::Impl {
public:
    std::reference_wrapper<AssetManager const> assets;
    std::vector<std::uint8_t> shaderBlob;
    ShaderPipelineStage pipelineStage;
    ShaderBytecode shaderBytecode;
    std::string entryPoint;
    std::optional<std::string> shaderFilePath;
    std::unique_ptr<Error> lastError;
    bool precompiled;
    bool fromDefaultLibrary;

public:
    explicit Impl(AssetManager& assets);

    std::shared_ptr<gpu::GraphicsDevice> GetDevice()
    {
        return assets.get().getGraphicsDevice();
    }

    /// Opens a stream for reading a file from the asset path.
    [[nodiscard]] std::tuple<std::ifstream, std::size_t, std::unique_ptr<Error>>
    OpenStream(const std::string& filePath) const;
};

Builder<Shader>::Impl::Impl(AssetManager& assetsIn)
    : assets(assetsIn)
    , pipelineStage(ShaderPipelineStage::VertexShader)
    , precompiled(false)
    , fromDefaultLibrary(false)
{
}

std::tuple<std::ifstream, std::size_t, std::unique_ptr<Error>>
Builder<Shader>::Impl::OpenStream(const std::string& filePath) const
{
    std::ifstream stream{filePath, std::ifstream::binary};

    if (!stream) {
        auto err = errors::make("cannot open the file, " + filePath);
        return std::make_tuple(std::move(stream), 0, std::move(err));
    }

    auto [byteLength, sizeErr] = FileSystem::getFileSize(filePath);
    if (sizeErr != nullptr) {
        auto err = errors::wrap(std::move(sizeErr), "failed to get file size, " + filePath);
        return std::make_tuple(std::move(stream), 0, std::move(err));
    }

    return std::make_tuple(std::move(stream), byteLength, nullptr);
}

Builder<Shader>::Builder(AssetManager& assetsIn, ShaderPipelineStage pipelineStageIn)
    : impl(std::make_shared<Impl>(assetsIn))
{
    POMDOG_ASSERT(impl);
    impl->pipelineStage = pipelineStageIn;
}

Builder<Shader>::~Builder() = default;

Builder<Shader>&
Builder<Shader>::setGLSL(const void* shaderSourceIn, std::size_t byteLengthIn)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(shaderSourceIn != nullptr);
    POMDOG_ASSERT(byteLengthIn > 0);

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->getSupportedLanguage() == ShaderLanguage::GLSL) {
        impl->shaderBytecode.code = shaderSourceIn;
        impl->shaderBytecode.byteLength = byteLengthIn;
        impl->precompiled = false;
        impl->shaderFilePath = std::nullopt;
    }
    return *this;
}

Builder<Shader>&
Builder<Shader>::setGLSLFromFile(const std::string& assetName)
{
    POMDOG_ASSERT(!assetName.empty());

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->getSupportedLanguage() == ShaderLanguage::GLSL) {
        auto filePath = impl->assets.get().getAssetPath(assetName);

        auto [stream, byteLength, err] = impl->OpenStream(filePath);

        if (err != nullptr) {
            impl->lastError = errors::wrap(std::move(err), "failed to open file: " + filePath);
            return *this;
        }

        POMDOG_ASSERT(stream);

        if (byteLength <= 0) {
            impl->lastError = errors::wrap(std::move(err), "the file is too small: " + filePath);
            return *this;
        }

        impl->shaderBlob = BinaryReader::readArray<std::uint8_t>(stream, byteLength);

        if (impl->shaderBlob.empty()) {
            impl->lastError = errors::wrap(std::move(err), "the file is too small: " + filePath);
            return *this;
        }

        // NOTE: Using the #include in GLSL support
        std::set<std::string> includes;
        auto includeResult = IncludeGLSLFilesRecursive(filePath, includes);

        if (includeResult) {
            auto& shaderCode = *includeResult;
            impl->shaderBlob.clear();
            impl->shaderBlob.resize(shaderCode.size());
            std::memcpy(impl->shaderBlob.data(), shaderCode.data(), shaderCode.size());
        }

        // NOTE: Insert null at the end of a charater array
        impl->shaderBlob.push_back(0);

        impl->shaderBytecode.code = impl->shaderBlob.data();
        impl->shaderBytecode.byteLength = impl->shaderBlob.size() - 1;
        impl->shaderFilePath = filePath;
    }
    return *this;
}

Builder<Shader>&
Builder<Shader>::setHLSL(
    const void* shaderSourceIn,
    std::size_t byteLengthIn,
    const std::string& entryPointIn)
{
    POMDOG_ASSERT(shaderSourceIn != nullptr);
    POMDOG_ASSERT(byteLengthIn > 0);
    POMDOG_ASSERT(!entryPointIn.empty());

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->getSupportedLanguage() == ShaderLanguage::HLSL) {
        impl->shaderBytecode.code = shaderSourceIn;
        impl->shaderBytecode.byteLength = byteLengthIn;
        impl->entryPoint = entryPointIn;
        impl->precompiled = false;
        impl->shaderFilePath = std::nullopt;
    }
    return *this;
}

Builder<Shader>&
Builder<Shader>::setHLSLPrecompiled(const void* shaderSourceIn, std::size_t byteLengthIn)
{
    POMDOG_ASSERT(shaderSourceIn != nullptr);
    POMDOG_ASSERT(byteLengthIn > 0);

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->getSupportedLanguage() == ShaderLanguage::HLSL) {
        impl->shaderBytecode.code = shaderSourceIn;
        impl->shaderBytecode.byteLength = byteLengthIn;
        impl->precompiled = true;
        impl->shaderFilePath = std::nullopt;
    }
    return *this;
}

Builder<Shader>&
Builder<Shader>::setHLSLFromFile(const std::string& assetName, const std::string& entryPointIn)
{
    POMDOG_ASSERT(!assetName.empty());
    POMDOG_ASSERT(!entryPointIn.empty());

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->getSupportedLanguage() == ShaderLanguage::HLSL) {
        auto filePath = impl->assets.get().getAssetPath(assetName);

        auto [stream, byteLength, err] = impl->OpenStream(filePath);

        if (err != nullptr) {
            impl->lastError = errors::wrap(std::move(err), "failed to open file: " + filePath);
            return *this;
        }

        POMDOG_ASSERT(stream);

        if (byteLength <= 0) {
            impl->lastError = errors::wrap(std::move(err), "the file is too small: " + filePath);
            return *this;
        }

        impl->shaderBlob = BinaryReader::readArray<std::uint8_t>(stream, byteLength);

        if (impl->shaderBlob.empty()) {
            impl->lastError = errors::wrap(std::move(err), "the file is too small: " + filePath);
            return *this;
        }

        // NOTE: Insert null at the end of a charater array
        impl->shaderBlob.push_back(0);

        impl->shaderBytecode.code = impl->shaderBlob.data();
        impl->shaderBytecode.byteLength = impl->shaderBlob.size() - 1;
        impl->entryPoint = entryPointIn;
        impl->precompiled = false;
        impl->shaderFilePath = filePath;
    }
    return *this;
}

Builder<Shader>&
Builder<Shader>::setMetal(
    const void* shaderSourceIn,
    std::size_t byteLengthIn,
    const std::string& entryPointIn)
{
    POMDOG_ASSERT(shaderSourceIn != nullptr);
    POMDOG_ASSERT(byteLengthIn > 0);
    POMDOG_ASSERT(!entryPointIn.empty());

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->getSupportedLanguage() == ShaderLanguage::Metal) {
        impl->shaderBytecode.code = shaderSourceIn;
        impl->shaderBytecode.byteLength = byteLengthIn;
        impl->entryPoint = entryPointIn;
        impl->precompiled = false;
        impl->fromDefaultLibrary = false;
        impl->shaderFilePath = std::nullopt;
    }
    return *this;
}

Builder<Shader>&
Builder<Shader>::setMetalPrecompiled(
    const void* shaderSourceIn,
    std::size_t byteLengthIn,
    const std::string& entryPointIn)
{
    POMDOG_ASSERT(shaderSourceIn != nullptr);
    POMDOG_ASSERT(byteLengthIn > 0);
    POMDOG_ASSERT(!entryPointIn.empty());

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->getSupportedLanguage() == ShaderLanguage::Metal) {
        impl->shaderBytecode.code = shaderSourceIn;
        impl->shaderBytecode.byteLength = byteLengthIn;
        impl->entryPoint = entryPointIn;
        impl->precompiled = true;
        impl->fromDefaultLibrary = false;
        impl->shaderFilePath = std::nullopt;
    }
    return *this;
}

Builder<Shader>&
Builder<Shader>::setMetalFromFile(const std::string& assetName, const std::string& entryPointIn)
{
    POMDOG_ASSERT(!assetName.empty());
    POMDOG_ASSERT(!entryPointIn.empty());

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->getSupportedLanguage() == ShaderLanguage::Metal) {
        auto filePath = impl->assets.get().getAssetPath(assetName);

        auto [stream, byteLength, err] = impl->OpenStream(filePath);

        if (err != nullptr) {
            impl->lastError = errors::wrap(std::move(err), "failed to open file: " + filePath);
            return *this;
        }

        POMDOG_ASSERT(stream);

        if (byteLength <= 0) {
            impl->lastError = errors::wrap(std::move(err), "the file is too small: " + filePath);
            return *this;
        }

        impl->shaderBlob = BinaryReader::readArray<std::uint8_t>(stream, byteLength);

        if (impl->shaderBlob.empty()) {
            impl->lastError = errors::wrap(std::move(err), "the file is too small: " + filePath);
            return *this;
        }

        // NOTE: Insert null at the end of a charater array
        impl->shaderBlob.push_back(0);

        impl->shaderBytecode.code = impl->shaderBlob.data();
        impl->shaderBytecode.byteLength = impl->shaderBlob.size() - 1;
        impl->entryPoint = entryPointIn;
        impl->precompiled = false;
        impl->fromDefaultLibrary = false;
        impl->shaderFilePath = filePath;
    }
    return *this;
}

Builder<Shader>&
Builder<Shader>::setMetalFromPrecompiledFile(const std::string& assetName, const std::string& entryPointIn)
{
    POMDOG_ASSERT(!assetName.empty());
    POMDOG_ASSERT(!entryPointIn.empty());

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->getSupportedLanguage() == ShaderLanguage::Metal) {
        auto filePath = impl->assets.get().getAssetPath(assetName);

        auto [stream, byteLength, err] = impl->OpenStream(filePath);

        if (err != nullptr) {
            impl->lastError = errors::wrap(std::move(err), "failed to open file: " + filePath);
            return *this;
        }

        POMDOG_ASSERT(stream);

        if (byteLength <= 0) {
            impl->lastError = errors::wrap(std::move(err), "the file is too small: " + filePath);
            return *this;
        }

        impl->shaderBlob = BinaryReader::readArray<std::uint8_t>(stream, byteLength);

        if (impl->shaderBlob.empty()) {
            impl->lastError = errors::wrap(std::move(err), "the file is too small: " + filePath);
            return *this;
        }

        impl->shaderBytecode.code = impl->shaderBlob.data();
        impl->shaderBytecode.byteLength = impl->shaderBlob.size();
        impl->entryPoint = entryPointIn;
        impl->precompiled = true;
        impl->fromDefaultLibrary = false;
        impl->shaderFilePath = filePath;
    }
    return *this;
}

Builder<Shader>&
Builder<Shader>::setMetalFromLibrary(const std::string& entryPointIn)
{
    POMDOG_ASSERT(!entryPointIn.empty());

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (graphicsDevice->getSupportedLanguage() == ShaderLanguage::Metal) {
        impl->shaderBytecode.code = nullptr;
        impl->shaderBytecode.byteLength = 0;
        impl->entryPoint = entryPointIn;
        impl->precompiled = false;
        impl->fromDefaultLibrary = true;
    }
    return *this;
}

std::tuple<std::shared_ptr<Shader>, std::unique_ptr<Error>>
Builder<Shader>::build()
{
    if (impl->lastError != nullptr) {
        return std::make_tuple(nullptr, std::move(impl->lastError));
    }

    auto graphicsDevice = impl->GetDevice();
    POMDOG_ASSERT(graphicsDevice);

    const auto shaderLanguage = graphicsDevice->getSupportedLanguage();
    std::optional<std::string> currentDirectory;
    if (impl->shaderFilePath) {
        currentDirectory = filepaths::normalize(
            filepaths::getDirectoryName(*impl->shaderFilePath));
    }

    switch (shaderLanguage) {
    case ShaderLanguage::GLSL: {
        POMDOG_ASSERT(impl->shaderBytecode.code != nullptr);
        POMDOG_ASSERT(impl->shaderBytecode.byteLength > 0);
        return GLSLCompiler::CreateShader(
            *graphicsDevice,
            impl->shaderBytecode.code,
            impl->shaderBytecode.byteLength,
            impl->pipelineStage,
            std::move(currentDirectory));
    }
    case ShaderLanguage::HLSL: {
        POMDOG_ASSERT(impl->shaderBytecode.code != nullptr);
        POMDOG_ASSERT(impl->shaderBytecode.byteLength > 0);
        if (impl->precompiled) {
            return HLSLCompiler::CreateShaderFromBinary(
                *graphicsDevice,
                impl->shaderBytecode.code,
                impl->shaderBytecode.byteLength,
                impl->pipelineStage);
        }
        POMDOG_ASSERT(!impl->entryPoint.empty());
        return HLSLCompiler::CreateShaderFromSource(
            *graphicsDevice,
            impl->shaderBytecode.code,
            impl->shaderBytecode.byteLength,
            impl->entryPoint,
            impl->pipelineStage,
            std::move(currentDirectory));
    }
    case ShaderLanguage::Metal: {
        POMDOG_ASSERT(!impl->entryPoint.empty());
        if (impl->fromDefaultLibrary) {
            return MetalCompiler::CreateShaderFromDefaultLibrary(
                *graphicsDevice,
                impl->entryPoint,
                impl->pipelineStage);
        }
        POMDOG_ASSERT(impl->shaderBytecode.code != nullptr);
        POMDOG_ASSERT(impl->shaderBytecode.byteLength > 0);
        if (impl->precompiled) {
            return MetalCompiler::CreateShaderFromBinary(
                *graphicsDevice,
                impl->shaderBytecode.code,
                impl->shaderBytecode.byteLength,
                impl->entryPoint,
                impl->pipelineStage);
        }
        return MetalCompiler::CreateShaderFromSource(
            *graphicsDevice,
            impl->shaderBytecode.code,
            impl->shaderBytecode.byteLength,
            impl->entryPoint,
            impl->pipelineStage);
    }
    }

    return std::make_tuple(nullptr, errors::make("this shading language is not supported"));
}

} // namespace pomdog::AssetBuilders
