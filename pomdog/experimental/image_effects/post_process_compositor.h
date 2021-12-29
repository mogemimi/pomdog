// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/experimental/image_effects/image_effect_base.h"
#include "pomdog/experimental/image_effects/screen_quad.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/math/rectangle.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT PostProcessCompositor final {
private:
    std::vector<std::shared_ptr<ImageEffectPreRenderable>> preRenderables;
    std::vector<std::shared_ptr<ImageEffectBase>> imageEffects;
    std::array<std::shared_ptr<gpu::RenderTarget2D>, 2> renderTargets;
    std::shared_ptr<gpu::DepthStencilBuffer> depthStencilBuffer;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer;
    ScreenQuad screenQuad;
    Rectangle viewport;

public:
    explicit PostProcessCompositor(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    void Composite(
        std::vector<std::shared_ptr<ImageEffectBase>>&& imageEffects);

    void Composite(
        std::vector<std::shared_ptr<ImageEffectBase>>&& imageEffects,
        std::vector<std::shared_ptr<ImageEffectPreRenderable>>&& preRenderableEffects);

    void SetViewportSize(
        gpu::GraphicsDevice& graphicsDevice,
        int width,
        int height,
        PixelFormat depthFormat);

    void Draw(
        gpu::CommandList& commandList,
        const std::shared_ptr<gpu::RenderTarget2D>& source);

    void Draw(
        gpu::CommandList& commandList,
        const std::shared_ptr<gpu::RenderTarget2D>& source,
        const std::shared_ptr<gpu::RenderTarget2D>& destination);

    bool CanSkipPostProcess() const noexcept;

private:
    void BuildRenderTargets(
        gpu::GraphicsDevice& graphicsDevice,
        int width,
        int height,
        PixelFormat surfaceFormat,
        PixelFormat depthFormat);

    void UpdateConstantBuffer();
};

} // namespace pomdog
