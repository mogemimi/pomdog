// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

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
        const std::shared_ptr<GraphicsCommandList>& commandList) override;

    void Draw(
        const std::shared_ptr<GraphicsCommandList>& commandList,
        RenderCommand & command) override;

    void End(const std::shared_ptr<GraphicsCommandList>& commandList) override;

    int GetDrawCallCount() const noexcept override;

    void SetViewProjection(
        const Matrix4x4& view, const Matrix4x4& projection) override;

    std::type_index GetCommandType() const noexcept override;

public:
    Matrix4x4 viewProjection;
    SpriteBatchRenderer spriteBatch;
    int drawCallCount;
};

} // namespace Rendering
} // namespace Pomdog
