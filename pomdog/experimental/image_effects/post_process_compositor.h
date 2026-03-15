// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/image_effects/image_effect_base.h"
#include "pomdog/experimental/image_effects/screen_quad.h"
#include "pomdog/math/rect2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class ConstantBuffer;
class DepthStencilBuffer;
class GraphicsDevice;
class RenderTarget2D;
enum class PixelFormat : u8;
} // namespace pomdog::gpu

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog {

class POMDOG_EXPORT PostProcessCompositor final {
private:
    std::vector<std::shared_ptr<ImageEffectPreRenderable>> preRenderables_;
    std::vector<std::shared_ptr<ImageEffectBase>> imageEffects_;
    std::array<std::shared_ptr<gpu::RenderTarget2D>, 2> renderTargets_;
    std::shared_ptr<gpu::DepthStencilBuffer> depthStencilBuffer_;
    std::shared_ptr<gpu::ConstantBuffer> constantBuffer_;
    ScreenQuad screenQuad_;
    Rect2D viewport_;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    void composite(
        std::vector<std::shared_ptr<ImageEffectBase>>&& imageEffects);

    void composite(
        std::vector<std::shared_ptr<ImageEffectBase>>&& imageEffects,
        std::vector<std::shared_ptr<ImageEffectPreRenderable>>&& preRenderableEffects);

    [[nodiscard]] std::unique_ptr<Error>
    setViewportSize(
        gpu::GraphicsDevice& graphicsDevice,
        int width,
        int height,
        gpu::PixelFormat depthFormat);

    void draw(
        gpu::CommandList& commandList,
        const std::shared_ptr<gpu::RenderTarget2D>& source);

    void draw(
        gpu::CommandList& commandList,
        const std::shared_ptr<gpu::RenderTarget2D>& source,
        const std::shared_ptr<gpu::RenderTarget2D>& destination);

    bool canSkipPostProcess() const noexcept;

private:
    [[nodiscard]] std::unique_ptr<Error>
    buildRenderTargets(
        gpu::GraphicsDevice& graphicsDevice,
        int width,
        int height,
        gpu::PixelFormat surfaceFormat,
        gpu::PixelFormat depthFormat);

    void updateConstantBuffer();
};

} // namespace pomdog
