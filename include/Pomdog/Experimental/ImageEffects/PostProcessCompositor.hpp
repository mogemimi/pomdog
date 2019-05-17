// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/ImageEffects/ImageEffectBase.hpp"
#include "Pomdog/Experimental/ImageEffects/ScreenQuad.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include <array>
#include <memory>
#include <vector>

namespace Pomdog {

class PostProcessCompositor final {
private:
    std::vector<std::shared_ptr<ImageEffectPreRenderable>> preRenderables;
    std::vector<std::shared_ptr<ImageEffectBase>> imageEffects;
    std::array<std::shared_ptr<RenderTarget2D>, 2> renderTargets;
    std::shared_ptr<ConstantBuffer> constantBuffer;
    ScreenQuad screenQuad;
    Rectangle viewport;

public:
    explicit PostProcessCompositor(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    void Composite(
        std::vector<std::shared_ptr<ImageEffectBase>> && imageEffects);

    void Composite(
        std::vector<std::shared_ptr<ImageEffectBase>> && imageEffects,
        std::vector<std::shared_ptr<ImageEffectPreRenderable>> && preRenderableEffects);

    void SetViewportSize(
        GraphicsDevice & graphicsDevice,
        int width,
        int height,
        DepthFormat depthFormat);

    void Draw(
        GraphicsCommandList & commandList,
        const std::shared_ptr<RenderTarget2D>& source);

    void Draw(
        GraphicsCommandList & commandList,
        const std::shared_ptr<RenderTarget2D>& source,
        const std::shared_ptr<RenderTarget2D>& destination);

    bool CanSkipPostProcess() const noexcept;

private:
    void BuildRenderTargets(
        GraphicsDevice & graphicsDevice,
        int width,
        int height,
        SurfaceFormat surfaceFormat,
        DepthFormat depthFormat);

    void UpdateConstantBuffer();
};

} // namespace Pomdog
