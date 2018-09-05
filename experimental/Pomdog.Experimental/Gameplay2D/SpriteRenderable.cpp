// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Gameplay2D/SpriteRenderable.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform.hpp"
#include "Pomdog.Experimental/Rendering/Renderer.hpp"
#include "Pomdog/Experimental/Graphics/SpriteBatch.hpp"
#include <limits>

namespace Pomdog {
namespace {

namespace SpriteRenderableDirtyFlags {

constexpr std::uint32_t OriginPivot = 0b001;
constexpr std::uint32_t Region = 0b010;

} // namespace SpriteRenderableDirtyFlags

/*
Matrix3x2 CreateTransformMatrix3x2(const Transform& transform)
{
    return Matrix3x2::CreateScale(transform.GetScale2D())
        * Matrix3x2::CreateRotation(transform.GetRotation2D())
        * Matrix3x2::CreateTranslation(transform.GetPosition2D());
}
*/

//TextureRegion CreateTextureRegionFromTexture(std::shared_ptr<Texture2D> const& texture)
//{
//    POMDOG_ASSERT(texture);
//    TextureRegion textureRegion;
//    textureRegion.Subrect.X = 0;
//    textureRegion.Subrect.Y = 0;
//    textureRegion.Subrect.Width = texture->Width();
//    textureRegion.Subrect.Height = texture->Height();
//    textureRegion.XOffset = 0;
//    textureRegion.YOffset = 0;
//    textureRegion.Width = texture->Width();
//    textureRegion.Height = texture->Height();
//    textureRegion.Rotate = false;
//    return std::move(textureRegion);
//}

} // unnamed namespace

SpriteRenderable::SpriteRenderable()
    : originPivot(0.5f, 0.5f)
    , color(Color::White)
    , dirtyFlags(std::numeric_limits<std::uint32_t>::max())
{
    command.SetDrawOrder(0.0f);
    offsetMatrix = Matrix3x2::Identity;
    textureRegion.X = 0;
    textureRegion.Y = 0;
    textureRegion.Width = 1;
    textureRegion.Height = 1;
}

std::shared_ptr<Texture2D> SpriteRenderable::GetTexture()
{
    return texture;
}

void SpriteRenderable::SetTexture(const std::shared_ptr<Texture2D>& textureIn)
{
    POMDOG_ASSERT(textureIn);
    texture = textureIn;
    dirtyFlags |= SpriteRenderableDirtyFlags::OriginPivot;
    dirtyFlags |= SpriteRenderableDirtyFlags::Region;
}

const Rectangle& SpriteRenderable::GetTextureRegion()
{
    return textureRegion;
}

void SpriteRenderable::SetTextureRegion(const Rectangle& textureRegionIn)
{
    textureRegion = textureRegionIn;
    dirtyFlags |= SpriteRenderableDirtyFlags::Region;

    POMDOG_ASSERT(textureRegion.X >= 0);
    POMDOG_ASSERT(textureRegion.Y >= 0);
    POMDOG_ASSERT(textureRegion.Width > 0);
    POMDOG_ASSERT(textureRegion.Height > 0);
}

Vector2 SpriteRenderable::GetOriginPivot()
{
    return originPivot;
}

void SpriteRenderable::SetOriginPivot(const Vector2& originPivotIn)
{
    originPivot = originPivotIn;
    dirtyFlags |= SpriteRenderableDirtyFlags::OriginPivot;
}

Color SpriteRenderable::GetColor()
{
    return command.GetColor();
}

void SpriteRenderable::SetColor(const Color& colorIn)
{
    command.SetColor(colorIn);
}

void SpriteRenderable::Visit(Entity & entity, Renderer & renderer)
{
/*
    if (!IsVisible()) {
        return;
    }

    if (!texture) {
        return;
    }

    POMDOG_ASSERT(texture);

    dirtyFlags = 0;

    auto transform = entity.GetComponent<Transform>();
    if (!transform) {
        return;
    }
    auto scale = transform->GetScale();
    if (scale.X == 0.0f || scale.Y == 0.0f) {
        return;
    }

    command.Draw(
        texture,
        offsetMatrix * CreateTransformMatrix3x2(*transform),
        textureRegion,
        originPivotForDrawCommand);

    command.SetDrawOrder(GetDrawOrder());
    renderer.PushCommand(command);
*/
}

std::uint8_t ComponentTypeDeclaration<SpriteRenderable>::GetTypeIndex()
{
    return Detail::Gameplay::ComponentTypeIndex::Index<GraphicsComponent>();
}

std::shared_ptr<Component> ComponentCreator<SpriteRenderable>::CreateComponent()
{
    auto component = std::make_shared<SpriteRenderable>();
    return component;
}

std::uint8_t ComponentCreator<SpriteRenderable>::GetComponentType()
{
    return ComponentTypeDeclaration<SpriteRenderable>::GetTypeIndex();
}

} // namespace Pomdog
