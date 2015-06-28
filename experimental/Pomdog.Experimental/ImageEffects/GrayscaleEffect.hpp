// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GRAYSCALEEFFECT_C2865F07_HPP
#define POMDOG_GRAYSCALEEFFECT_C2865F07_HPP

#include "ImageEffectBase.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class GrayscaleEffect final: public ImageEffectBase {
public:
    GrayscaleEffect(
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets);

    void Apply(
        GraphicsCommandList & commandList,
        std::shared_ptr<RenderTarget2D> const& source,
        std::shared_ptr<ConstantBuffer> const& constantBuffer) override;

private:
    std::shared_ptr<SamplerState> samplerLinear;
    std::shared_ptr<PipelineState> pipelineState;
};

} // namespace Pomdog

#endif // POMDOG_GRAYSCALEEFFECT_C2865F07_HPP
