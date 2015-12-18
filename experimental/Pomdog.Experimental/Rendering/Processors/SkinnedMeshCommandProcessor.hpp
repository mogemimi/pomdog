// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Graphics/SkinnedEffect.hpp"
#include "Pomdog.Experimental/Rendering/RenderCommandProcessor.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class SkinnedMeshCommandProcessor final: public RenderCommandProcessor {
public:
    SkinnedMeshCommandProcessor(
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets);

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

} // namespace Pomdog
