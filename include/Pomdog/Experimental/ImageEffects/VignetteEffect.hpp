// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Experimental/ImageEffects/ImageEffectBase.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class POMDOG_EXPORT VignetteEffect final : public ImageEffectBase {
public:
    VignetteEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager& assets);

    void SetIntensity(float intensity);

    void Apply(
        GraphicsCommandList& commandList,
        const std::shared_ptr<RenderTarget2D>& source,
        const std::shared_ptr<ConstantBuffer>& constantBuffer) override;

private:
    std::shared_ptr<SamplerState> samplerLinear;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<ConstantBuffer> constantBufferVignette;
};

} // namespace Pomdog
