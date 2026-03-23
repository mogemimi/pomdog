// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/shader_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/gpu/graphics_backend.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_compilers/glsl_compiler.h"
#include "pomdog/gpu/shader_compilers/hlsl_compiler.h"
#include "pomdog/gpu/shader_compilers/metal_compiler.h"
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
using gpu::GraphicsBackend;

std::tuple<std::shared_ptr<gpu::Shader>, std::unique_ptr<Error>>
loadShaderFromFile(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    gpu::ShaderPipelineStage pipelineStage,
    const std::string& filePath,
    const std::string& entryPoint,
    const std::vector<u8>& reflectionBlob)
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

    const auto backendKind = graphicsDevice->getBackendKind();
    auto currentDirectory = filepaths::normalize(filepaths::getDirectoryName(filePath));

    switch (backendKind) {
    case GraphicsBackend::OpenGL4:
    case GraphicsBackend::WebGL: {
        const void* reflectPtr = reflectionBlob.empty() ? nullptr : reflectionBlob.data();
        const std::size_t reflectLen = reflectionBlob.size();
        return GLSLCompiler::createShader(
            *graphicsDevice,
            shaderBlob.data(),
            shaderBlob.size(),
            pipelineStage,
            std::move(currentDirectory),
            reflectPtr,
            reflectLen);
    }
    case GraphicsBackend::Direct3D11: {
        return HLSLCompiler::createShaderFromBinary(
            *graphicsDevice,
            shaderBlob.data(),
            shaderBlob.size(),
            pipelineStage);
    }
    case GraphicsBackend::Metal: {
        // NOTE: Insert null at the end of a character array
        shaderBlob.push_back(0);

        POMDOG_ASSERT(!entryPoint.empty());
        return MetalCompiler::createShaderFromSource(
            *graphicsDevice,
            shaderBlob.data(),
            shaderBlob.size() - 1,
            entryPoint,
            pipelineStage);
    }
    case GraphicsBackend::Vulkan: {
        // TODO: Implement Vulkan shader loading
        return std::make_tuple(nullptr, errors::make("Vulkan shader loading is not implemented yet"));
    }
    }

    return std::make_tuple(nullptr, errors::make("this shading language is not supported"));
}

} // namespace

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

    const auto backendKind = graphicsDevice->getBackendKind();
    std::string actualFilePath = {};

    switch (backendKind) {
    case GraphicsBackend::OpenGL4:
    case GraphicsBackend::WebGL:
        actualFilePath = join(directory, "glsl", filePath + ".glsl");
        break;
    case GraphicsBackend::Direct3D11:
        actualFilePath = join(directory, "d3d11", filePath + ".dxbc");
        break;
    case GraphicsBackend::Metal:
        actualFilePath = join(directory, "metal", filePath + ".metal");
        break;
    case GraphicsBackend::Vulkan:
        actualFilePath = join(directory, "vulkan", filePath + ".spv");
        break;
    }

    if (actualFilePath.empty()) {
        return std::make_tuple(nullptr, errors::make("this shading language is not supported"));
    }

    // NOTE: Load companion .reflect file for GL/WebGL backends
    std::vector<u8> reflectionBlob;
    if (backendKind == GraphicsBackend::OpenGL4 || backendKind == GraphicsBackend::WebGL) {
        auto reflectPath = join(directory, "reflect", filePath + ".reflect");
        if (auto [reflectFile, reflectOpenErr] = vfs::open(fs, reflectPath); reflectOpenErr == nullptr) {
            if (auto [reflectInfo, reflectStatErr] = reflectFile->stat(); reflectStatErr == nullptr && reflectInfo.size > 0) {
                reflectionBlob.resize(reflectInfo.size);
                auto [reflectBytesRead, reflectReadErr] = reflectFile->read(std::span<u8>{reflectionBlob.data(), reflectionBlob.size()});
                if (reflectReadErr != nullptr) {
                    reflectionBlob.clear();
                }
                else {
                    reflectionBlob.resize(reflectBytesRead);
                }
            }
        }
    }

    return loadShaderFromFile(fs, graphicsDevice, pipelineStage, actualFilePath, entryPoint, reflectionBlob);
}

} // namespace pomdog
