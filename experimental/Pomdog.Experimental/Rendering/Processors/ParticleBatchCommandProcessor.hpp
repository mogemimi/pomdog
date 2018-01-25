// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Graphics/SpriteBatchRenderer.hpp"
#include "Pomdog.Experimental/Rendering/RenderCommandProcessor.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class ParticleBatchCommandProcessor final : public RenderCommandProcessor {
public:
    ParticleBatchCommandProcessor(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager & assets);

    void Begin(GraphicsCommandQueue & commandQueue) override;

    void Draw(GraphicsCommandQueue & commandQueue, RenderCommand & command) override;

    void End(GraphicsCommandQueue & commandQueue) override;

    int GetDrawCallCount() const noexcept override;

    void SetViewProjection(const Matrix4x4& view, const Matrix4x4& projection) override;

public:
    SpriteBatchRenderer spriteBatch;
    std::uint32_t drawCallCount;
};

} // namespace Pomdog
