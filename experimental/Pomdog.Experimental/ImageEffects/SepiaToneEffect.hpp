// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SEPIATONEEFFECT_BD1A4ABD_HPP
#define POMDOG_SEPIATONEEFFECT_BD1A4ABD_HPP

#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class SepiaToneEffect {
public:
    explicit SepiaToneEffect(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets);

    void SetViewport(float width, float height);
    void SetTexture(std::shared_ptr<RenderTarget2D> const& texture);

    void Apply(GraphicsContext & graphicsContext);

private:
    std::shared_ptr<RenderTarget2D> texture;
    std::shared_ptr<SamplerState> samplerLinear;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<ConstantBufferBinding> constantBuffers;
};

}// namespace Pomdog

#endif // POMDOG_SEPIATONEEFFECT_BD1A4ABD_HPP
