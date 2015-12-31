// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/DrawingContext.hpp"
#include "Pomdog.Experimental/Graphics/SpriteBatchRenderer.hpp"
#include "Pomdog.Experimental/Graphics/SpriteFont.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>
#include <vector>

namespace Pomdog {
namespace UI {

class SpriteDrawingContext final : public UI::DrawingContext {
private:
    SpriteBatchRenderer spriteBatch;
    std::shared_ptr<SpriteFont> spriteFont;
    std::shared_ptr<Texture2D> texture;
    std::vector<Matrix3x2> matrixStack;
    int viewportHeight;

public:
    SpriteDrawingContext(
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets,
        std::shared_ptr<SpriteFont> const& spriteFontIn,
        std::shared_ptr<Texture2D> const& textureIn);

    Matrix3x2 Top() const override;

    void Push(Matrix3x2 const& matrix) override;

    void Pop() override;

    void Begin(
        std::shared_ptr<GraphicsCommandList> const& commandList,
        Matrix4x4 const& matrix,
        int viewportHeight);

    void End();

    void DrawRectangle(
        Matrix3x2 const& transform,
        Color const& color,
        Rectangle const& rectangle) override;

    void DrawLine(
        Matrix3x2 const& transform,
        Color const& color,
        float penSize,
        Vector2 const& point1,
        Vector2 const& point2) override;

    void DrawString(
        Matrix3x2 const& transform,
        Color const& color,
        FontWeight fontWeight,
        FontSize fontSize,
        std::string const& text) override;
};

} // namespace UI
} // namespace Pomdog
