// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CHROMATICABERRATION_DEEDC0FF_HPP
#define POMDOG_CHROMATICABERRATION_DEEDC0FF_HPP

#include "ImageEffectBase.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class ChromaticAberration final: public ImageEffectBase {
public:
    ChromaticAberration(
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets);

    void BindConstantBuffer(
        std::shared_ptr<ConstantBuffer> const& constantBuffer) override;

    void Apply(
        GraphicsCommandList & commandList,
        std::shared_ptr<RenderTarget2D> const& source) override;

private:
    std::shared_ptr<SamplerState> samplerState;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<ConstantBufferBinding> constantBuffers;
};

} // namespace Pomdog

#endif // POMDOG_CHROMATICABERRATION_DEEDC0FF_HPP
