// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTPASSLOADER_C9744EEA_HPP
#define POMDOG_EFFECTPASSLOADER_C9744EEA_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <string>
#include <memory>
#include <vector>

namespace Pomdog {
namespace Detail {
class AssetLoaderContext;
}// namespace Detail

namespace AssetLoaders {

class POMDOG_EXPORT EffectPassLoader final {
public:
    explicit EffectPassLoader(Detail::AssetLoaderContext const& loaderContext);

    EffectPassLoader(EffectPassLoader &&);
    EffectPassLoader & operator=(EffectPassLoader &&);

    ~EffectPassLoader();

    EffectPassLoader & VertexShaderGLSL(std::string const& filePath);

    EffectPassLoader & PixelShaderGLSL(std::string const& filePath);

    EffectPassLoader & VertexShaderHLSL(std::string const& filePath, std::string const& entryPoint);

    EffectPassLoader & PixelShaderHLSL(std::string const& filePath, std::string const& entryPoint);

    EffectPassLoader & InputLayout(InputLayoutDescription const& inputLayout);

    EffectPassLoader & InputLayout(InputLayoutDescription && inputLayout);

    std::shared_ptr<EffectPass> Load();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}// namespace AssetLoaders
}// namespace Pomdogs

#endif // POMDOG_EFFECTPASSLOADER_C9744EEA_HPP
