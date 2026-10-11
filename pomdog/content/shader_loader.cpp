// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/shader_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/gpu/backends/shader_compile_options.h"
#include "pomdog/gpu/graphics_backend.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_pipeline_stage.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/path_helper.h"
#include "pomdog/utility/string_helper.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
#include <span>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

using gpu::GraphicsBackend;
using gpu::detail::ShaderCompileOptions;

std::tuple<std::shared_ptr<gpu::Shader>, std::unique_ptr<Error>>
loadShaderFromFile(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    gpu::ShaderPipelineStage pipelineStage,
    const std::string& filePath,
    const std::string& entryPoint,
    std::span<const u8> reflectionBlob)
{
    POMDOG_ASSERT(fs != nullptr);
    POMDOG_ASSERT(graphicsDevice != nullptr);
    POMDOG_ASSERT(!filePath.empty());

    auto [file, openErr] = vfs::open(fs, filePath);
    if (openErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(openErr), "failed to open file: " + filePath));
    }

    constexpr std::size_t maxFileSize = 256ULL * 1024 * 1024; // 256 MB
    auto [size, sizeErr] = file->getSize();
    if (sizeErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(sizeErr), "failed to get file size: " + filePath));
    }
    if (size > maxFileSize) {
        return std::make_tuple(nullptr, errors::make("the shader file is too large: " + filePath));
    }

    auto [shaderBlob, viewErr] = file->view();
    if (viewErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(viewErr), "failed to read file: " + filePath));
    }

    if (shaderBlob.empty()) {
        return std::make_tuple(nullptr, errors::make("the file is too small: " + filePath));
    }

    if (shaderBlob.size() > maxFileSize) {
        return std::make_tuple(nullptr, errors::make("the shader file is too large: " + filePath));
    }

    const auto backendKind = graphicsDevice->getBackendKind();

    switch (backendKind) {
    case GraphicsBackend::OpenGL4:
    case GraphicsBackend::WebGL: {
        ShaderCompileOptions compileOptions = {};
        compileOptions.entryPoint = "main";
        compileOptions.profile.pipelineStage = pipelineStage;
        compileOptions.precompiled = false;
        compileOptions.reflectionBlob = reflectionBlob;

        return graphicsDevice->createShader(shaderBlob, std::move(compileOptions));
    }
    case GraphicsBackend::Direct3D11: {
        ShaderCompileOptions compileOptions = {};
        compileOptions.profile.pipelineStage = pipelineStage;
        compileOptions.precompiled = true;

        return graphicsDevice->createShader(shaderBlob, std::move(compileOptions));
    }
    case GraphicsBackend::Metal: {
        // NOTE: The Metal backend reads the source as a null-terminated string,
        // so copy the source and insert null at the end of it.
        std::vector<u8> source(shaderBlob.begin(), shaderBlob.end());
        source.push_back(0);

        POMDOG_ASSERT(!entryPoint.empty());

        ShaderCompileOptions compileOptions = {};
        compileOptions.entryPoint = entryPoint;
        compileOptions.profile.pipelineStage = pipelineStage;
        compileOptions.precompiled = false;

        return graphicsDevice->createShader(
            std::span<const u8>{source}.first(shaderBlob.size()),
            std::move(compileOptions));
    }
    case GraphicsBackend::Vulkan: {
        ShaderCompileOptions compileOptions = {};
        compileOptions.profile.pipelineStage = pipelineStage;
        compileOptions.precompiled = true;

        return graphicsDevice->createShader(shaderBlob, std::move(compileOptions));
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
        actualFilePath = join(directory, "glsl", filePath + ".glsl");
        break;
    case GraphicsBackend::WebGL:
        actualFilePath = join(directory, "webgl", filePath + ".glsl");
        break;
    case GraphicsBackend::Direct3D11:
        actualFilePath = join(directory, "d3d11", filePath + ".dxbc");
        break;
    case GraphicsBackend::Metal:
        actualFilePath = join(directory, "metal", filePath + ".metal");
        break;
    case GraphicsBackend::Vulkan:
        actualFilePath = join(directory, "vk", filePath + ".spv");
        break;
    }

    if (actualFilePath.empty()) {
        return std::make_tuple(nullptr, errors::make("this shading language is not supported"));
    }

    // NOTE: The GL and WebGL backends need the reflection data that the shader
    // pipeline writes next to each GLSL shader, so a missing file is an error.
    std::shared_ptr<vfs::File> reflectFile = {};
    if (backendKind == GraphicsBackend::OpenGL4 || backendKind == GraphicsBackend::WebGL) {
        auto reflectPath = join(directory, "reflect", filePath + ".reflect");
        auto [file, openErr] = vfs::open(fs, reflectPath);
        if (openErr != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(openErr), "failed to open shader reflection file: " + reflectPath));
        }
        reflectFile = std::move(file);
    }

    // NOTE: The reflection data stays valid while reflectFile is open.
    std::span<const u8> reflectionBlob = {};
    if (reflectFile != nullptr) {
        auto [bytes, viewErr] = reflectFile->view();
        if (viewErr != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(viewErr), "failed to read shader reflection file for " + filePath));
        }
        if (bytes.empty()) {
            return std::make_tuple(nullptr, errors::make("the shader reflection file is empty for " + filePath));
        }
        reflectionBlob = bytes;
    }

    return loadShaderFromFile(fs, graphicsDevice, pipelineStage, actualFilePath, entryPoint, reflectionBlob);
}

} // namespace pomdog
