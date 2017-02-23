// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Graphics/SkinnedEffect.hpp"
#include "Pomdog.Experimental/Rendering/RenderCommandProcessor.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class SkinnedMeshCommandProcessor final : public RenderCommandProcessor {
public:
    SkinnedMeshCommandProcessor(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager & assets);

    void Begin(GraphicsCommandQueue & commandQueue) override;

    void Draw(GraphicsCommandQueue & commandQueue, RenderCommand & command) override;

    void End(GraphicsCommandQueue & commandQueue) override;

    int GetDrawCallCount() const noexcept override;

    void SetViewProjection(const Matrix4x4& view, const Matrix4x4& projection) override;

public:
    SkinnedEffect skinnedEffect;
    Matrix4x4 viewProjection;
    std::uint32_t drawCallCount;
};

} // namespace Pomdog
