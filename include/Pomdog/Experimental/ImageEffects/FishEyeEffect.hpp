// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "ImageEffectBase.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class FishEyeEffect final : public ImageEffectBase {
public:
    FishEyeEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager & assets);

    void SetStrength(float strength);

    void Apply(
        GraphicsCommandList & commandList,
        const std::shared_ptr<RenderTarget2D>& source,
        const std::shared_ptr<ConstantBuffer>& constantBuffer) override;

private:
    std::shared_ptr<SamplerState> samplerLinear;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<ConstantBuffer> constantBufferFishEye;
};

} // namespace Pomdog
