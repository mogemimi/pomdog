// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Experimental/ImageEffects/ImageEffectBase.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT SepiaToneEffect final : public ImageEffectBase {
public:
    SepiaToneEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager& assets);

    void Apply(
        GraphicsCommandList& commandList,
        const std::shared_ptr<RenderTarget2D>& source,
        const std::shared_ptr<ConstantBuffer>& constantBuffer) override;

private:
    std::shared_ptr<SamplerState> samplerLinear;
    std::shared_ptr<PipelineState> pipelineState;
};

} // namespace Pomdog
