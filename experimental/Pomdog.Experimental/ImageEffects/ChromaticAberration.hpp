// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CHROMATICABERRATION_DEEDC0FF_HPP
#define POMDOG_CHROMATICABERRATION_DEEDC0FF_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class ChromaticAberration {
public:
    explicit ChromaticAberration(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    void SetViewport(float width, float height);

    void SetTexture(std::shared_ptr<RenderTarget2D> const& texture);

    void Apply(GraphicsContext & graphicsContext);

private:
    std::shared_ptr<SamplerState> samplerState;
    std::shared_ptr<RenderTarget2D> texture;
    std::shared_ptr<EffectPass> effectPass;
    std::shared_ptr<ConstantBufferBinding> constantBuffers;
};

}// namespace Pomdog

#endif // POMDOG_CHROMATICABERRATION_DEEDC0FF_HPP
