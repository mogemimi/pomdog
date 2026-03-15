// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/shader_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/gpu/backends/shader_bytecode.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_compilers/glsl_compiler.h"
#include "pomdog/gpu/shader_compilers/hlsl_compiler.h"
#include "pomdog/gpu/shader_compilers/metal_compiler.h"
#include "pomdog/gpu/shader_language.h"
#include "pomdog/gpu/shader_pipeline_stage.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/path_helper.h"
#include "pomdog/utility/string_helper.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
#include <regex>
#include <set>
#include <span>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

using namespace gpu::shader_compilers;
using gpu::ShaderLanguage;

} // namespace

std::tuple<std::shared_ptr<gpu::Shader>, std::unique_ptr<Error>>
createShaderFromSource(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    gpu::ShaderPipelineStage pipelineStage,
    const void* shaderSource,
    std::size_t byteLength,
    const std::string& entryPoint)
{
    POMDOG_ASSERT(graphicsDevice != nullptr);
    POMDOG_ASSERT(shaderSource != nullptr);
    POMDOG_ASSERT(byteLength > 0);

    const auto shaderLanguage = graphicsDevice->getSupportedLanguage();

    switch (shaderLanguage) {
    case ShaderLanguage::GLSL:
        return GLSLCompiler::CreateShader(
            *graphicsDevice,
            shaderSource,
            byteLength,
            pipelineStage,
            std::nullopt);
    case ShaderLanguage::HLSL:
        POMDOG_ASSERT(!entryPoint.empty());
        return HLSLCompiler::CreateShaderFromSource(
            *graphicsDevice,
            shaderSource,
            byteLength,
            entryPoint,
            pipelineStage,
            std::nullopt);
    case ShaderLanguage::Metal:
        POMDOG_ASSERT(!entryPoint.empty());
        return MetalCompiler::CreateShaderFromSource(
            *graphicsDevice,
            shaderSource,
            byteLength,
            entryPoint,
            pipelineStage);
    }

    return std::make_tuple(nullptr, errors::make("this shading language is not supported"));
}

std::tuple<std::shared_ptr<gpu::Shader>, std::unique_ptr<Error>>
createShaderFromBinary(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    gpu::ShaderPipelineStage pipelineStage,
    const void* shaderBinary,
    std::size_t byteLength,
    const std::string& entryPoint)
{
    POMDOG_ASSERT(graphicsDevice != nullptr);
    POMDOG_ASSERT(shaderBinary != nullptr);
    POMDOG_ASSERT(byteLength > 0);

    const auto shaderLanguage = graphicsDevice->getSupportedLanguage();

    switch (shaderLanguage) {
    case ShaderLanguage::GLSL:
        return std::make_tuple(nullptr, errors::make("GLSL does not support precompiled shaders"));
    case ShaderLanguage::HLSL:
        return HLSLCompiler::CreateShaderFromBinary(
            *graphicsDevice,
            shaderBinary,
            byteLength,
            pipelineStage);
    case ShaderLanguage::Metal:
        POMDOG_ASSERT(!entryPoint.empty());
        return MetalCompiler::CreateShaderFromBinary(
            *graphicsDevice,
            shaderBinary,
            byteLength,
            entryPoint,
            pipelineStage);
    }

    return std::make_tuple(nullptr, errors::make("this shading language is not supported"));
}

std::tuple<std::shared_ptr<gpu::Shader>, std::unique_ptr<Error>>
loadShaderFromFile(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    gpu::ShaderPipelineStage pipelineStage,
    const std::string& filePath,
    const std::string& entryPoint)
{
    POMDOG_ASSERT(fs != nullptr);
    POMDOG_ASSERT(graphicsDevice != nullptr);
    POMDOG_ASSERT(!filePath.empty());

    auto [file, openErr] = vfs::open(fs, filePath);
    if (openErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(openErr), "failed to open file: " + filePath));
    }

    auto [fileInfo, statErr] = file->stat();
    if (statErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(statErr), "failed to get file info: " + filePath));
    }

    if (fileInfo.size == 0) {
        return std::make_tuple(nullptr, errors::make("the file is too small: " + filePath));
    }

    std::vector<u8> shaderBlob(fileInfo.size);
    auto [bytesRead, readErr] = file->read(std::span<u8>{shaderBlob.data(), shaderBlob.size()});
    if (readErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(readErr), "failed to read file: " + filePath));
    }
    shaderBlob.resize(bytesRead);

    if (shaderBlob.empty()) {
        return std::make_tuple(nullptr, errors::make("the file is too small: " + filePath));
    }

    const auto shaderLanguage = graphicsDevice->getSupportedLanguage();
    auto currentDirectory = filepaths::normalize(filepaths::getDirectoryName(filePath));

    switch (shaderLanguage) {
    case ShaderLanguage::GLSL: {
        return GLSLCompiler::CreateShader(
            *graphicsDevice,
            shaderBlob.data(),
            shaderBlob.size(),
            pipelineStage,
            std::move(currentDirectory));
    }
    case ShaderLanguage::HLSL: {
        return HLSLCompiler::CreateShaderFromBinary(
            *graphicsDevice,
            shaderBlob.data(),
            shaderBlob.size(),
            pipelineStage);
    }
    case ShaderLanguage::Metal: {
        // NOTE: Insert null at the end of a character array
        shaderBlob.push_back(0);

        POMDOG_ASSERT(!entryPoint.empty());
        return MetalCompiler::CreateShaderFromSource(
            *graphicsDevice,
            shaderBlob.data(),
            shaderBlob.size() - 1,
            entryPoint,
            pipelineStage);
    }
    }

    return std::make_tuple(nullptr, errors::make("this shading language is not supported"));
}

[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<gpu::Shader>, std::unique_ptr<Error>>
loadShaderAutomagically(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    gpu::ShaderPipelineStage pipelineStage,
    const std::string& directory,
    const std::string& filePath,
    const std::string& entryPoint)
{
    POMDOG_ASSERT(fs != nullptr);
    POMDOG_ASSERT(graphicsDevice != nullptr);
    POMDOG_ASSERT(!filePath.empty());

    const auto join = [](std::string_view dir, std::string_view kind, std::string_view filename) -> std::string {
        return filepaths::joinUnix(filepaths::joinUnix(dir, kind), filename);
    };

    const auto shaderLanguage = graphicsDevice->getSupportedLanguage();
    std::string actualFilePath = {};

    switch (shaderLanguage) {
    case ShaderLanguage::GLSL:
        actualFilePath = join(directory, "glsl", filePath + ".glsl");
        break;
    case ShaderLanguage::HLSL:
        actualFilePath = join(directory, "d3d11", filePath + ".dxbc");
        break;
    case ShaderLanguage::Metal:
        actualFilePath = join(directory, "metal", filePath + ".metal");
        break;
    }

    if (actualFilePath.empty()) {
        return std::make_tuple(nullptr, errors::make("this shading language is not supported"));
    }

    return loadShaderFromFile(fs, graphicsDevice, pipelineStage, actualFilePath, entryPoint);
}

} // namespace pomdog
