// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/AssetBuilders/Builder.hpp"
#include "Pomdog/Graphics/ShaderPipelineStage.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace Pomdog {
class AssetManager;
} // namespace Pomdog

namespace Pomdog::AssetBuilders {

template <>
class POMDOG_EXPORT Builder<Shader> final {
public:
    Builder(AssetManager& assets, ShaderPipelineStage pipelineStage);

    Builder(Builder&&) = default;
    Builder(const Builder&) = default;
    Builder& operator=(Builder&&) = default;
    Builder& operator=(const Builder&) = default;

    ~Builder();

    /// Sets the string of the GLSL source code, and the entry point of the function.
    Builder& SetGLSL(const void* shaderSource, std::size_t byteLength);

    /// Sets the file path to GLSL file, and the entry point of the function.
    Builder& SetGLSLFromFile(const std::string& filePath);

    /// Sets the string of the HLSL source code, and the entry point of the function.
    Builder& SetHLSL(
        const void* shaderSource,
        std::size_t byteLength,
        const std::string& entryPoint);

    /// Sets the data from a precompiled HLSL binary, and the entry point of the function.
    Builder& SetHLSLPrecompiled(
        const void* shaderSource,
        std::size_t byteLength);

    /// Sets the file path to HLSL file, and the entry point of the function.
    Builder& SetHLSLFromFile(
        const std::string& filePath,
        const std::string& entryPoint);

    /// Sets the string of the Metal source code, and the entry point of the function.
    Builder& SetMetal(
        const void* shaderSource,
        std::size_t byteLength,
        const std::string& entryPoint);

    /// Sets the data from a precompiled Metal library, and the entry point of the function.
    Builder& SetMetalPrecompiled(
        const void* shaderSource,
        std::size_t byteLength,
        const std::string& entryPoint);

    /// Sets the file path to `.metal` file, and the entry point of the function.
    Builder& SetMetalFromFile(
        const std::string& filePath,
        const std::string& entryPoint);

    /// Sets the file path to `.metallib` file, and the entry point of the function.
    Builder& SetMetalFromPrecompiledFile(
        const std::string& filePath,
        const std::string& entryPoint);

    /// Sets the entry point of the function in the default library.
    Builder& SetMetalFromLibrary(const std::string& entryPoint);

    /// Returns an object representing shader function.
    [[nodiscard]] std::shared_ptr<Shader> Build();

private:
    class Impl;
    std::shared_ptr<Impl> impl;
};

} // namespace Pomdog::AssetBuilders
