// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Graphics/SpriteBatchRenderer.hpp"
#include "Pomdog.Experimental/Rendering/RenderCommandProcessor.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"

namespace Pomdog {
namespace Rendering {

class SpriteBatchCommandProcessor final : public RenderCommandProcessor {
public:
    SpriteBatchCommandProcessor(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager & assets);

    void Begin(
        const std::shared_ptr<GraphicsCommandList>& commandList,
        const Matrix4x4& viewProjection) override;

    void Draw(
        const std::shared_ptr<GraphicsCommandList>& commandList,
        RenderCommand & command) override;

    void FlushBatch() override;

    void End() override;

    int GetDrawCallCount() const noexcept override;

    std::type_index GetCommandType() const noexcept override;

public:
    SpriteBatchRenderer spriteBatch;
};

} // namespace Rendering
} // namespace Pomdog
