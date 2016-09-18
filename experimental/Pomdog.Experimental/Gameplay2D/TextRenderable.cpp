// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Gameplay2D/TextRenderable.hpp"
#include "Pomdog.Experimental/Rendering/Renderer.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform.hpp"
#include "Pomdog.Experimental/Graphics/SpriteFont.hpp"

namespace Pomdog {

TextRenderable::TextRenderable()
    : originPivot(0.5f, 0.5f)
    , textColor(Color::Black)
{
    command.drawOrder = 0;
}

void TextRenderable::Visit(Entity & entity, Renderer & renderer)
{
    if (!IsVisible()) {
        return;
    }

    if (text.empty()) {
        return;
    }

    if (!spriteFont) {
        return;
    }

    Vector2 position = Vector2::Zero;
    Vector2 scale = {1.0f, 1.0f};
    if (auto transform = entity.GetComponent<Transform>()) {
        //command.worldMatrix = transform->GetTransformMatrix();
        position = transform->GetPosition2D();
        scale.X = transform->GetScale().X;
        scale.Y = transform->GetScale().Y;
    }
    else {
        //command.worldMatrix = Matrix4x4::Identity;
        position = Vector2::Zero;
        scale.X = 1.0f;
        scale.Y = 1.0f;
    }

    command.onDraw = [this, position, scale](SpriteBatchRenderer & spriteBatch) {
        spriteFont->Draw(spriteBatch, text, position, textColor, 0.0f, scale);
    };

    command.drawOrder = GetDrawOrder();
    renderer.PushCommand(command);
}

void TextRenderable::SetFont(const std::shared_ptr<SpriteFont>& fontIn)
{
    POMDOG_ASSERT(fontIn);
    spriteFont = fontIn;
}

void TextRenderable::SetText(const std::string& textIn)
{
    text = textIn;
}

void TextRenderable::SetTextColor(const Color& textColorIn)
{
    textColor = textColorIn;
}

std::uint8_t ComponentTypeDeclaration<TextRenderable>::GetTypeIndex()
{
    return Detail::Gameplay::ComponentTypeIndex::Index<GraphicsComponent>();
}

std::shared_ptr<Component> ComponentCreator<TextRenderable>::CreateComponent()
{
    auto component = std::make_shared<TextRenderable>();
    return component;
}

std::uint8_t ComponentCreator<TextRenderable>::GetComponentType()
{
    return ComponentTypeDeclaration<TextRenderable>::GetTypeIndex();
}

} // namespace Pomdog
