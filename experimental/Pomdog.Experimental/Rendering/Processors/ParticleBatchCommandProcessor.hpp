// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PARTICLEBATCHCOMMANDPROCESSOR_29CECBB2_4602_44F1_B146_121E3E118ABF_HPP
#define POMDOG_PARTICLEBATCHCOMMANDPROCESSOR_29CECBB2_4602_44F1_B146_121E3E118ABF_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog.Experimental/Graphics/SpriteBatchRenderer.hpp"
#include "Pomdog.Experimental/Rendering/RenderCommandProcessor.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class ParticleBatchCommandProcessor final: public RenderCommandProcessor {
public:
    ParticleBatchCommandProcessor(std::shared_ptr<GraphicsContext> const& graphicsContext,
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

#endif // !defined(POMDOG_PARTICLEBATCHCOMMANDPROCESSOR_29CECBB2_4602_44F1_B146_121E3E118ABF_HPP)
