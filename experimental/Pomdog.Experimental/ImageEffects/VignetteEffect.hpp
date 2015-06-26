// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_VIGNETTEEFFECT_395F49A8_HPP
#define POMDOG_VIGNETTEEFFECT_395F49A8_HPP

#include "ImageEffectBase.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class VignetteEffect final: public ImageEffectBase {
public:
    VignetteEffect(
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets);

    void SetIntensity(float intensity);

    void BindConstantBuffer(
        std::shared_ptr<ConstantBuffer> const& constantBuffer) override;

    void Apply(
        GraphicsCommandList & commandList,
        std::shared_ptr<RenderTarget2D> const& source) override;

private:
    std::shared_ptr<SamplerState> samplerLinear;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<ConstantBufferBinding> constantBuffers;
};

} // namespace Pomdog

#endif // POMDOG_VIGNETTEEFFECT_395F49A8_HPP
