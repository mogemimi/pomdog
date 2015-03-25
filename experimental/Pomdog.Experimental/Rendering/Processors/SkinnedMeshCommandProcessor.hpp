// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SKINNEDMESHCOMMANDPROCESSOR_40F7E4BF_DF7A_41B0_8F8C_FE6FE6FA38FE_HPP
#define POMDOG_SKINNEDMESHCOMMANDPROCESSOR_40F7E4BF_DF7A_41B0_8F8C_FE6FE6FA38FE_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog.Experimental/Graphics/SkinnedEffect.hpp"
#include "Pomdog.Experimental/Rendering/RenderCommandProcessor.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class SkinnedMeshCommandProcessor final: public RenderCommandProcessor {
public:
    explicit SkinnedMeshCommandProcessor(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    void Begin(GraphicsContext & graphicsContext) override;

    void Draw(GraphicsContext & graphicsContext, RenderCommand & command) override;

    void End(GraphicsContext & graphicsContext) override;

    int DrawCallCount() const override;

    void SetViewProjection(Matrix4x4 const& view, Matrix4x4 const& projection) override;

public:
    SkinnedEffect skinnedEffect;
    Matrix4x4 viewProjection;
    std::uint32_t drawCallCount;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKINNEDMESHCOMMANDPROCESSOR_40F7E4BF_DF7A_41B0_8F8C_FE6FE6FA38FE_HPP)
