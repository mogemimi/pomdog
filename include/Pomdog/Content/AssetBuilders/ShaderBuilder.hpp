// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SHADERBUILDER_DF696479_HPP
#define POMDOG_SHADERBUILDER_DF696479_HPP

#include "Pomdog/Content/AssetBuilders/Builder.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/ShaderPipelineStage.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <cstddef>
#include <string>
#include <memory>

namespace Pomdog {

namespace Detail {
class AssetLoaderContext;
} // namespace Detail

namespace AssetBuilders {

template <>
class POMDOG_EXPORT Builder<Shader> {
public:
    Builder(
        Detail::AssetLoaderContext const& loaderContext,
        ShaderPipelineStage pipelineStage);

    Builder(Builder &&) = default;
    Builder(Builder const&) = default;
    Builder & operator=(Builder &&) = default;
    Builder & operator=(Builder const&) = default;

    ~Builder();

    Builder & SetGLSL(void const* shaderSource, std::size_t byteLength);

    Builder & SetGLSLFromFile(std::string const& filePath);

    Builder & SetHLSL(void const* shaderSource, std::size_t byteLength,
        std::string const& entryPoint);

    Builder & SetHLSLPrecompiled(void const* shaderSource,
        std::size_t byteLength);

    Builder & SetHLSLFromFile(std::string const& filePath,
        std::string const& entryPoint);

    std::unique_ptr<Shader> Build();

private:
    class Impl;
    std::shared_ptr<Impl> impl;
};

} // namespace AssetBuilders
} // namespace Pomdog

#endif // POMDOG_SHADERBUILDER_DF696479_HPP
