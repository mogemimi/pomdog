// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_POSTPROCESSCOMPOSITOR_CF09DF9E_HPP
#define POMDOG_POSTPROCESSCOMPOSITOR_CF09DF9E_HPP

#include "ImageEffectBase.hpp"
#include "../Graphics/ScreenQuad.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include <vector>
#include <array>
#include <memory>

namespace Pomdog {

class PostProcessCompositor {
private:
    std::vector<std::shared_ptr<ImageEffectBase>> imageEffects;
    std::array<std::shared_ptr<RenderTarget2D>, 2> renderTargets;
    std::shared_ptr<ConstantBuffer> constantBuffer;
    ScreenQuad screenQuad;
    Rectangle viewport;

public:
    PostProcessCompositor(
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        int width, int height, SurfaceFormat surfaceFormat);

    void Composite(std::vector<std::shared_ptr<ImageEffectBase>> && imageEffects);

    void SetViewportSize(
        GraphicsDevice & graphicsDevice, int width, int height);

    void Draw(
        GraphicsCommandList & commandList,
        std::shared_ptr<RenderTarget2D> const& source);

    void Draw(
        GraphicsCommandList & commandList,
        std::shared_ptr<RenderTarget2D> const& source,
        std::shared_ptr<RenderTarget2D> const& destination);

private:
    void BuildRenderTargets(
        GraphicsDevice & graphicsDevice,
        int width,
        int height,
        SurfaceFormat surfaceFormat);

    void UpdateConstantBuffer();
};

} // namespace Pomdog

#endif // POMDOG_POSTPROCESSCOMPOSITOR_CF09DF9E_HPP
