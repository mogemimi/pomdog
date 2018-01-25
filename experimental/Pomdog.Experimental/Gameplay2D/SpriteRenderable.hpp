// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/detail/ComponentTypeIndex.hpp"
#include "Pomdog.Experimental/Gameplay2D/GraphicsComponent.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SpriteBatchCommand.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Experimental/TexturePacker/TextureRegion.hpp"
#include <Pomdog/Pomdog.hpp>
#include <functional>
#include <memory>

namespace Pomdog {

class SpriteRenderable final : public GraphicsComponent {
public:
    SpriteRenderable();

    void Visit(Entity & entity, Renderer & renderer) override;

    std::shared_ptr<Texture2D> GetTexture();

    void SetTexture(const std::shared_ptr<Texture2D>& texture);

    const Rectangle& GetTextureRegion();

    void SetTextureRegion(const Rectangle& textureRegion);

    Vector2 GetOriginPivot();

    void SetOriginPivot(const Vector2& originPivot);

    Color GetColor();

    void SetColor(const Color& color);

private:
    std::shared_ptr<Texture2D> texture;
    Matrix3x2 offsetMatrix;
    Matrix3x2 transformForDrawCommand;
    Rectangle textureRegion;
    Pomdog::Rendering::SpriteBatchCommand command;
    Vector2 originPivot;
    Vector2 originPivotForDrawCommand;
    Color color;
    std::uint32_t dirtyFlags;

//    bool rotate;
//    Detail::Rendering::SpriteCommand command;
//    Matrix3x2 offsetMatrix;
};

template <>
struct ComponentTypeDeclaration<SpriteRenderable> final {
    static std::uint8_t GetTypeIndex();
};

template <>
class ComponentCreator<SpriteRenderable> final : public ComponentCreatorBase {
public:
    std::shared_ptr<Component> CreateComponent() override;

    std::uint8_t GetComponentType() override;
};

} // namespace Pomdog
