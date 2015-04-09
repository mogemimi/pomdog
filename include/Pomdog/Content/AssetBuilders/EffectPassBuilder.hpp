// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTPASSBUILDER_0DAFFCC3_HPP
#define POMDOG_EFFECTPASSBUILDER_0DAFFCC3_HPP

#include "Pomdog/Content/AssetBuilders/Builder.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <string>
#include <memory>

namespace Pomdog {
namespace Detail {
class AssetLoaderContext;
} // namespace Detail

namespace AssetBuilders {

template <>
class POMDOG_EXPORT Builder<EffectPass> {
public:
    explicit Builder(Detail::AssetLoaderContext const& loaderContext);

    Builder(Builder &&);

    Builder & operator=(Builder &&);

    ~Builder();

    Builder & SetVertexShader(std::unique_ptr<Shader> && vertexShader);

    Builder & SetPixelShader(std::unique_ptr<Shader> && pixelShader);

    Builder & SetInputLayout(InputLayoutDescription const& inputLayout);

    Builder & SetInputLayout(InputLayoutDescription && inputLayout);

    Builder & SetBlendState(BlendDescription const& blendState);

    Builder & SetRasterizerState(RasterizerDescription const& rasterizerState);

    Builder & SetDepthStencilState(DepthStencilDescription const& depthStencilState);

    std::shared_ptr<EffectPass> Build();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace AssetBuilders
} // namespace Pomdogs

#endif // POMDOG_EFFECTPASSBUILDER_0DAFFCC3_HPP
