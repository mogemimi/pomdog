// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/detail/ComponentTypeIndex.hpp"
#include "Pomdog.Experimental/Gameplay2D/GraphicsComponent.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SpriteBatchCommand.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class SpriteFont;

class TextRenderable final : public GraphicsComponent {
public:
    TextRenderable();

    void Visit(Entity & entity, Renderer & renderer) override;

    void SetFont(const std::shared_ptr<SpriteFont>& font);

    void SetText(const std::string& text);

    void SetTextColor(const Color& textcolor);

private:
    std::shared_ptr<SpriteFont> spriteFont;
    Pomdog::Rendering::SpriteFontCommand command;
    Vector2 originPivot;
    std::string text;
    Color textColor;
};

template <>
struct ComponentTypeDeclaration<TextRenderable> final {
    static std::uint8_t GetTypeIndex();
};

template <>
class ComponentCreator<TextRenderable> final : public ComponentCreatorBase {
public:
    std::shared_ptr<Component> CreateComponent() override;

    std::uint8_t GetComponentType() override;
};

} // namespace Pomdog
