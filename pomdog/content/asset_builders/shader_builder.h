// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/content/asset_builders/builder.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/shader_pipeline_stage.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class AssetManager;
class Error;
} // namespace pomdog

namespace pomdog::AssetBuilders {

template <>
class POMDOG_EXPORT Builder<gpu::Shader> final {
public:
    Builder(AssetManager& assets, gpu::ShaderPipelineStage pipelineStage);

    Builder(Builder&&) = default;
    Builder(const Builder&) = default;
    Builder& operator=(Builder&&) = default;
    Builder& operator=(const Builder&) = default;

    ~Builder();

    /// Sets the string of the GLSL source code, and the entry point of the function.
    Builder& setGLSL(const void* shaderSource, std::size_t byteLength);

    /// Sets the file path to GLSL file, and the entry point of the function.
    Builder& setGLSLFromFile(const std::string& filePath);

    /// Sets the string of the HLSL source code, and the entry point of the function.
    Builder& setHLSL(
        const void* shaderSource,
        std::size_t byteLength,
        const std::string& entryPoint);

    /// Sets the data from a precompiled HLSL binary, and the entry point of the function.
    Builder& setHLSLPrecompiled(
        const void* shaderSource,
        std::size_t byteLength);

    /// Sets the file path to HLSL file, and the entry point of the function.
    Builder& setHLSLFromFile(
        const std::string& filePath,
        const std::string& entryPoint);

    /// Sets the string of the Metal source code, and the entry point of the function.
    Builder& setMetal(
        const void* shaderSource,
        std::size_t byteLength,
        const std::string& entryPoint);

    /// Sets the data from a precompiled Metal library, and the entry point of the function.
    Builder& setMetalPrecompiled(
        const void* shaderSource,
        std::size_t byteLength,
        const std::string& entryPoint);

    /// Sets the file path to `.metal` file, and the entry point of the function.
    Builder& setMetalFromFile(
        const std::string& filePath,
        const std::string& entryPoint);

    /// Sets the file path to `.metallib` file, and the entry point of the function.
    Builder& setMetalFromPrecompiledFile(
        const std::string& filePath,
        const std::string& entryPoint);

    /// Sets the entry point of the function in the default library.
    Builder& setMetalFromLibrary(const std::string& entryPoint);

    /// Returns an object representing shader function.
    [[nodiscard]] std::tuple<std::shared_ptr<gpu::Shader>, std::unique_ptr<Error>>
    build();

private:
    class Impl;
    std::shared_ptr<Impl> impl;
};

} // namespace pomdog::AssetBuilders
