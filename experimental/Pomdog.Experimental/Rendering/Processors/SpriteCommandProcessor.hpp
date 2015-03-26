// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SPRITECOMMANDPROCESSOR_1D9B87F7_A4BD_4BC2_AD4C_48E683760A2E_HPP
#define POMDOG_SPRITECOMMANDPROCESSOR_1D9B87F7_A4BD_4BC2_AD4C_48E683760A2E_HPP

#include "Pomdog.Experimental/Graphics/SpriteBatchRenderer.hpp"
#include "Pomdog.Experimental/Rendering/RenderCommandProcessor.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class SpriteCommandProcessor final: public RenderCommandProcessor {
public:
    SpriteCommandProcessor(std::shared_ptr<GraphicsContext> const& graphicsContext,
        std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    void Begin(GraphicsContext & graphicsContext) override;

    void Draw(GraphicsContext & graphicsContext, RenderCommand & command) override;

    void End(GraphicsContext & graphicsContext) override;

    int DrawCallCount() const override;

    void SetViewProjection(Matrix4x4 const& view, Matrix4x4 const& projection) override;

public:
    Detail::Rendering::SpriteBatchRenderer spriteBatch;
    std::uint32_t drawCallCount;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SPRITECOMMANDPROCESSOR_1D9B87F7_A4BD_4BC2_AD4C_48E683760A2E_HPP)
