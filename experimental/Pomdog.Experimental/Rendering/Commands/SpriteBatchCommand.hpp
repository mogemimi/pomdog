// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Rendering/RenderCommand.hpp"
#include "Pomdog/Experimental/Graphics/SpriteBatch.hpp"
#include "Pomdog/Experimental/Graphics/SpriteFont.hpp"
#include "Pomdog/Experimental/TexturePacker/TextureRegion.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {
namespace Rendering {

class SpriteRenderCommand : public RenderCommand {
public:
    virtual ~SpriteRenderCommand() = default;

    virtual void Execute(SpriteBatch& spriteBatch) = 0;

    std::type_index GetType() const noexcept override final;
};

class SpriteBatchCommand final : public SpriteRenderCommand {
public:
    Color GetColor() const noexcept
    {
        return color;
    }

    void SetColor(const Color& colorIn) noexcept
    {
        color = colorIn;
    }

    void SetPosition(const Vector2& positionIn) noexcept
    {
        position = positionIn;
    }

    void SetRotation(const Radian<float>& rotationIn) noexcept
    {
        rotation = rotationIn;
    }

    void SetScale(const Vector2& scaleIn) noexcept
    {
        scale = scaleIn;
    }

    void SetScale(float scaleIn) noexcept
    {
        scale = Vector2{scaleIn, scaleIn};
    }

    void SetTexture(
        const std::shared_ptr<Texture2D>& textureIn,
        const TextureRegion& textureRegion,
        const Vector2& originPivotIn)
    {
        const auto regionSize = Vector2{
            static_cast<float>(textureRegion.Width),
            static_cast<float>(textureRegion.Height)
        };
        const auto regionOffset = Vector2{
            static_cast<float>(textureRegion.XOffset),
            static_cast<float>(textureRegion.YOffset)
        };
        const auto regionSubrectSize = Vector2{
            static_cast<float>(textureRegion.Subrect.Width),
            static_cast<float>(textureRegion.Subrect.Height)
        };

        texture = textureIn;
        sourceRect = textureRegion.Subrect;
        originPivot = (((originPivotIn * regionSize) - regionOffset) / regionSubrectSize);
    }

    void SetTexture(const std::shared_ptr<Texture2D>& textureIn)
    {
        texture = textureIn;
        sourceRect = Rectangle{0, 0, textureIn->GetWidth(), textureIn->GetHeight()};
        originPivot = Vector2{0.5f, 0.5f};
    }

    void SetTexture(
        const std::shared_ptr<Texture2D>& textureIn,
        const Rectangle& sourceRectIn,
        const Vector2& originPivotIn)
    {
        texture = textureIn;
        sourceRect = sourceRectIn;
        originPivot = originPivotIn;
    }

    void Execute(SpriteBatch& spriteBatch) override;

private:
    std::shared_ptr<Texture2D> texture;
    Vector2 position;
    Radian<float> rotation;
    Vector2 scale;
    Rectangle sourceRect;
    Color color;
    Vector2 originPivot;
};

class SpriteFontCommand final : public SpriteRenderCommand {
public:
    SpriteFontCommand()
        : position(Vector2::Zero)
        , color(Color::White)
        , rotation(0.0f)
        , scale(1.0f, 1.0f)
    {
    }

    void SetPosition(const Vector2& positionIn)
    {
        position = positionIn;
    }

    void SetRotation(const Radian<float>& rotationIn)
    {
        rotation = rotationIn;
    }

    void SetScale(const Vector2& scaleIn)
    {
        scale = scaleIn;
    }

    void SetColor(const Color& colorIn) noexcept
    {
        color = colorIn;
    }

    void SetFont(const std::shared_ptr<SpriteFont>& spriteFontIn)
    {
        spriteFont = spriteFontIn;
    }

    void SetText(const std::string& textIn)
    {
        text = textIn;
    }

    void Execute(SpriteBatch& spriteBatch) override;

private:
    std::shared_ptr<SpriteFont> spriteFont;
    std::string text;
    Vector2 position;
    Color color;
    Radian<float> rotation;
    Vector2 scale;
};

} // namespace Rendering
} // namespace Pomdog
