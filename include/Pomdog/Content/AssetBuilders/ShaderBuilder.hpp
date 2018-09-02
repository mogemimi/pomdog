// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

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

namespace Detail {
class AssetLoaderContext;
} // namespace Detail

namespace AssetBuilders {

template <>
class POMDOG_EXPORT Builder<Shader> final {
public:
    Builder(
        const Detail::AssetLoaderContext& loaderContext,
        ShaderPipelineStage pipelineStage);

    Builder(Builder&&) = default;
    Builder(const Builder&) = default;
    Builder& operator=(Builder&&) = default;
    Builder& operator=(const Builder&) = default;

    ~Builder();

    Builder& SetGLSL(const void* shaderSource, std::size_t byteLength);

    Builder& SetGLSLFromFile(const std::string& filePath);

    Builder& SetHLSL(
        const void* shaderSource,
        std::size_t byteLength,
        const std::string& entryPoint);

    Builder& SetHLSLPrecompiled(
        const void* shaderSource,
        std::size_t byteLength);

    Builder& SetHLSLFromFile(
        const std::string& filePath,
        const std::string& entryPoint);

    Builder& SetMetal(
        const void* shaderSource,
        std::size_t byteLength,
        const std::string& entryPoint);

    Builder& SetMetalFromFile(
        const std::string& filePath,
        const std::string& entryPoint);

    Builder& SetMetalFromLibrary(const std::string& entryPoint);

    std::shared_ptr<Shader> Build();

private:
    class Impl;
    std::shared_ptr<Impl> impl;
};

} // namespace AssetBuilders
} // namespace Pomdog
