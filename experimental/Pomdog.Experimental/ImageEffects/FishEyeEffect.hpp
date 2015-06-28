// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_FISHEYEEFFECT_62F772C0_HPP
#define POMDOG_FISHEYEEFFECT_62F772C0_HPP

#include "ImageEffectBase.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class FishEyeEffect final: public ImageEffectBase {
public:
    FishEyeEffect(
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets);

    void SetStrength(float strength);

    void Apply(
        GraphicsCommandList & commandList,
        std::shared_ptr<RenderTarget2D> const& source,
        std::shared_ptr<ConstantBuffer> const& constantBuffer) override;

private:
    std::shared_ptr<SamplerState> samplerLinear;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<ConstantBuffer> constantBufferFishEye;
};

} // namespace Pomdog

#endif // POMDOG_FISHEYEEFFECT_62F772C0_HPP
