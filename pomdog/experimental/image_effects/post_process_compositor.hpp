// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/experimental/image_effects/image_effect_base.hpp"
#include "pomdog/experimental/image_effects/screen_quad.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/math/rectangle.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class POMDOG_EXPORT PostProcessCompositor final {
private:
    std::vector<std::shared_ptr<ImageEffectPreRenderable>> preRenderables;
    std::vector<std::shared_ptr<ImageEffectBase>> imageEffects;
    std::array<std::shared_ptr<RenderTarget2D>, 2> renderTargets;
    std::shared_ptr<DepthStencilBuffer> depthStencilBuffer;
    std::shared_ptr<ConstantBuffer> constantBuffer;
    ScreenQuad screenQuad;
    Rectangle viewport;

public:
    explicit PostProcessCompositor(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    void Composite(
        std::vector<std::shared_ptr<ImageEffectBase>>&& imageEffects);

    void Composite(
        std::vector<std::shared_ptr<ImageEffectBase>>&& imageEffects,
        std::vector<std::shared_ptr<ImageEffectPreRenderable>>&& preRenderableEffects);

    void SetViewportSize(
        GraphicsDevice& graphicsDevice,
        int width,
        int height,
        SurfaceFormat depthFormat);

    void Draw(
        GraphicsCommandList& commandList,
        const std::shared_ptr<RenderTarget2D>& source);

    void Draw(
        GraphicsCommandList& commandList,
        const std::shared_ptr<RenderTarget2D>& source,
        const std::shared_ptr<RenderTarget2D>& destination);

    bool CanSkipPostProcess() const noexcept;

private:
    void BuildRenderTargets(
        GraphicsDevice& graphicsDevice,
        int width,
        int height,
        SurfaceFormat surfaceFormat,
        SurfaceFormat depthFormat);

    void UpdateConstantBuffer();
};

} // namespace Pomdog
