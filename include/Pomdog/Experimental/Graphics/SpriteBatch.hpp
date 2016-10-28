// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class AssetManager;

class POMDOG_EXPORT SpriteBatch final {
public:
    SpriteBatch(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager & assets);

    SpriteBatch(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const BlendDescription& blendDescription,
        AssetManager & assets);

    ~SpriteBatch();

    void Begin(
        const std::shared_ptr<GraphicsCommandList>& commandList,
        const Matrix4x4& transformMatrix);

    void Draw(
        const std::shared_ptr<Texture2D>& texture,
        const Rectangle& sourceRect,
        const Color& color);

    void Draw(
        const std::shared_ptr<Texture2D>& texture,
        const Vector2& position,
        const Color& color);

    void Draw(
        const std::shared_ptr<Texture2D>& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color);

    void Draw(
        const std::shared_ptr<Texture2D>& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        float scale);

    void Draw(
        const std::shared_ptr<Texture2D>& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

    void End();

    int GetDrawCallCount() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
